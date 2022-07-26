#include "pch.h"
#include "Panels/ContentsPanel.h"

#define MINI_CASE_SENSITIVE
#include <mINI/ini.h>
#include <nfd.hpp>

#include "Elements/AddXboxButton.h"
#include "Elements/Xbox.h"
#include "Panels/PathPanel.h"
#include "Xbox/XboxManager.h"
#include "Elements/File.h"
#include "Render/UI.h"

ContentsPanel::ContentsPanel()
{
    m_Elements.emplace_back(CreateRef<AddXboxButton>());

    struct stat buffer;
    std::string configFilePath = GetExecDir().append("OpenNeighborhood.ini").string();
    if (stat(configFilePath.c_str(), &buffer) != -1)
    {
        mINI::INIFile configFile(configFilePath);
        mINI::INIStructure config;
        configFile.read(config);

        for (const auto &it : config)
        {
            const std::string &consoleName = it.first;
            if (config.get(consoleName).has("ip_address"))
            {
                std::string ipAddress = config.get(consoleName).get("ip_address");
                m_Elements.emplace_back(CreateRef<Xbox>(consoleName, ipAddress));
            }
        }
    }
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

    if (XboxManager::GetCurrentLocation() != "")
        DisplayContextMenu();

    ImGuiStyle &style = ImGui::GetStyle();
    float panelWidth = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    for (size_t i = 0; i < m_Elements.size(); i++)
    {
        ImGui::PushID(static_cast<int>(i));
        m_Elements[i]->OnRender();
        float lastButtonX = ImGui::GetItemRectMax().x;
        float nextButtonX = lastButtonX + style.ItemSpacing.x + m_Elements[i]->GetWidth(); // Expected position if next button was on same line
        if (i + 1 < m_Elements.size() && nextButtonX < panelWidth)
            ImGui::SameLine();
        ImGui::PopID();
    }

    ImGui::End();

    if (!m_ContentsChangeEventQueue.empty())
        InjectNewElements();
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

    m_ContentsChangeEventQueue.pop();
}

void ContentsPanel::DisplayContextMenu()
{
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
    XBDM::Console &xbox = XboxManager::GetConsole();
    std::set<XBDM::File> files;
    const std::string &location = XboxManager::GetCurrentLocation();

    // If the current location is a drive (e.g hdd:), we need to append '\' to it
    bool success = XboxManager::Try([&]() { files = xbox.GetDirectoryContents(location.back() == ':' ? location + '\\' : location); });

    if (!success)
        return;

    auto fileElements = std::vector<Ref<Element>>();

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
    std::string remotePath = XboxManager::GetCurrentLocation() + '\\' + fileName;

    // It's important to capture remotePath and localPath by copy because they will
    // be destroyed by the time upload is called if it's called as the confirm
    // callback, capturing them by reference would create a crash.
    auto upload = [this, remotePath, localPath]() {
        XBDM::Console &xbox = XboxManager::GetConsole();

        bool success = XboxManager::Try([&]() { xbox.SendFile(remotePath, localPath.string()); });

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
        XBDM::Console &xbox = XboxManager::GetConsole();

        bool success = XboxManager::Try([&]() { xbox.CreateDirectory(XboxManager::GetCurrentLocation() + '\\' + name); });

        if (success)
            UpdateContents();
    };

    UI::SetInputTextCallback(createDirectory);
    UI::SetInputTextHeader("Enter a name");
    UI::DisplayInputText(true);
}
