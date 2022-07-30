#include "pch.h"
#include "Panels/ContentsPanel.h"

#include <nfd.hpp>

#include "Elements/AddXboxButton.h"
#include "Elements/Xbox.h"
#include "Helpers/ConsoleStore.h"
#include "Helpers/LocationMover.h"
#include "Helpers/ConfigManager.h"
#include "Elements/File.h"
#include "Render/UI.h"

ContentsPanel::ContentsPanel()
{
    m_Elements = UI::CreateRootElements();
}

void ContentsPanel::OnRender()
{
    float pathPanelHeight = m_Margin * 2.4f;

    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus;

    ImGui::SetNextWindowPos(ImVec2(m_Margin, m_Margin * 2.0f + pathPanelHeight));
    ImGui::SetNextWindowSize(ImVec2(m_WindowWidth - m_Margin * 2.0f, m_WindowHeight - (m_Margin * 3.0f + pathPanelHeight)));

    ImGui::Begin("Contents Window", nullptr, windowFlags);

    // Display the right click menu
    DisplayContextMenu();

    ImGuiStyle &style = ImGui::GetStyle();
    float panelWidth = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    for (size_t i = 0; i < m_Elements.size(); i++)
    {
        ImGui::PushID(static_cast<int>(i));
        m_Elements[i]->OnRender();
        float lastButtonX = ImGui::GetItemRectMax().x;
        float nextButtonX = lastButtonX + style.ItemSpacing.x + m_Elements[i]->GetWidth(); // Expected position if next button was on same line

        // Place elements on the same line as long as they are not the last element and that there is
        // enough space for them
        if (i + 1 < m_Elements.size() && nextButtonX < panelWidth)
            ImGui::SameLine();

        ImGui::PopID();
    }

    ImGui::End();

    if (!m_ContentsChangeEventQueue.empty())
    {
        InjectNewElements();
        m_ContentsChangeEventQueue.pop();
    }
}

void ContentsPanel::OnEvent(Event &event)
{
    Panel::OnEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<ContentsChangeEvent>(BIND_EVENT_FN(ContentsPanel::OnContentsChange));
}

bool ContentsPanel::OnContentsChange(ContentsChangeEvent &event)
{
    m_ContentsChangeEventQueue.push(event);

    return true;
}

void ContentsPanel::InjectNewElements()
{
    ContentsChangeEvent &event = m_ContentsChangeEventQueue.front();
    if (event.Append())
    {
        m_Elements.reserve(m_Elements.size() + event.GetElements().size());
        m_Elements.insert(m_Elements.end(), event.GetElements().begin(), event.GetElements().end());
    }
    else
        m_Elements = event.GetElements();
}

void ContentsPanel::DisplayContextMenu()
{
    // If we are not inside of a drive yet, we don't want to allow uploading nor creating a directory
    if (LocationMover::GetCurrentAppLocation() != LocationMover::AppLocation::DriveContents)
        return;

    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::Button("Upload Here"))
        {
            Upload();
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::Button("Create Directory"))
        {
            CreateDirectory();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void ContentsPanel::UpdateContents()
{
    XBDM::Console &console = ConsoleStore::GetConsole();
    std::set<XBDM::File> files;
    const std::string &consoleLocation = LocationMover::GetCurrentConsoleLocation();

    // If the current location is a drive (e.g hdd:), we need to append '\' to it
    bool success = ConsoleStore::Try([&]() { files = console.GetDirectoryContents(consoleLocation.back() == ':' ? consoleLocation + '\\' : consoleLocation); });

    if (!success)
        return;

    auto fileElements = std::vector<Ref<Element>>();
    fileElements.reserve(files.size());

    for (auto &file : files)
        fileElements.emplace_back(CreateRef<File>(file));

    ContentsChangeEvent event(fileElements);
    OnContentsChange(event);
}

void ContentsPanel::Upload()
{
    NFD::UniquePathN outPath;
    nfdresult_t result = NFD::OpenDialog(outPath);

    if (result == NFD_CANCEL || result == NFD_ERROR)
        return;

    std::filesystem::path localPath = outPath.get();
    std::string fileName = localPath.filename().string();
    std::string remotePath = LocationMover::GetCurrentConsoleLocation() + '\\' + fileName;

    // It's important to capture remotePath and localPath by copy because they will
    // be destroyed by the time upload is called if it's called as the confirm
    // callback, capturing them by reference would create a crash.
    auto upload = [this, remotePath, localPath]() {
        XBDM::Console &console = ConsoleStore::GetConsole();

        bool success = ConsoleStore::Try([&]() { console.SendFile(remotePath, localPath.string()); });

        if (success)
            UpdateContents();
    };

    UI::SetConfirmCallback(upload);

    auto fileAlreadyExists = std::find_if(m_Elements.begin(), m_Elements.end(), [&](const Ref<Element> &element) {
        return element->GetLabel() == fileName;
    });

    if (fileAlreadyExists != m_Elements.end())
    {
        UI::SetConfirmMessage("A file named \"" + fileName + "\" already exists, do you want to overwrite it?");
        UI::SetConfirm(true);
        return;
    }

    upload();
}

void ContentsPanel::CreateDirectory()
{
    auto createDirectory = [this](const std::string &name) {
        XBDM::Console &console = ConsoleStore::GetConsole();

        bool success = ConsoleStore::Try([&]() { console.CreateDirectory(LocationMover::GetCurrentConsoleLocation() + '\\' + name); });

        if (success)
            UpdateContents();
    };

    UI::SetInputTextCallback(createDirectory);
    UI::SetInputTextHeader("Enter a name");
    UI::DisplayInputText(true);
}
