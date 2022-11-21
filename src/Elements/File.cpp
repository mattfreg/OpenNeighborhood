#include "pch.h"
#include "Elements/File.h"

#include <nfd.hpp>

#include "Core/PlatformDetector.h"
#include "Helpers/ConsoleStore.h"
#include "Helpers/LocationMover.h"
#include "Helpers/NumberFormatter.h"
#include "Events/AppEvent.h"
#include "Render/UI.h"

File::File(const XBDM::File &data)
    : Element(data.Name, data.IsDirectory ? "directory" : data.IsXex ? "xex" : "file"), m_Data(data)
{
}

void File::OnClick()
{
    if (m_Data.IsDirectory)
        OpenDirectory();
    else if (m_Data.IsXex)
        LaunchXex();
}

void File::OpenDirectory()
{
    XBDM::Console &console = ConsoleStore::GetConsole();
    std::set<XBDM::File> files;

    bool success = ConsoleStore::Try([&]() { files = console.GetDirectoryContents(LocationMover::GetCurrentConsoleLocation() + '\\' + m_Data.Name); });

    if (!success)
        return;

    LocationMover::GoToDirectory(m_Data.Name);

    auto fileElements = std::vector<Ref<Element>>();
    fileElements.reserve(files.size());

    for (auto &file : files)
        fileElements.emplace_back(CreateRef<File>(file));

    ContentsChangeEvent event(fileElements);
    m_EventCallback(event);
}

void File::LaunchXex()
{
    XBDM::Console &console = ConsoleStore::GetConsole();
    console.LaunchXex(LocationMover::GetCurrentConsoleLocation() + '\\' + m_Data.Name);
}

void File::Download()
{
    // Depending on the system, std::filesystem::path::native can return either
    // std::wstring or std::string. Since we don't know, we are just using auto.
    auto fileName = std::filesystem::path(m_Data.Name);
    auto extension = fileName.extension().native();

    // Oven the save dialog that will get a path to where to save the current file
    NFD::UniquePathN outPath;
    nfdresult_t result = NFD_ERROR;

    if (!extension.empty())
    {
        // Remove the "." from the extension
        extension = extension.substr(1);

        // The filter name is the extension but uppercase
        auto filterName = extension;
        std::transform(filterName.begin(), filterName.end(), filterName.begin(), ::toupper);

        nfdnfilteritem_t filterItem[] = { { filterName.c_str(), extension.c_str() } };
        result = NFD::SaveDialog(outPath, filterItem, 1, nullptr, fileName.native().c_str());
    }
    else
        result = NFD::SaveDialog(outPath, nullptr, 0, nullptr, fileName.native().c_str());

    if (result == NFD_ERROR)
    {
        UI::SetErrorMessage(NFD::GetError());
        UI::SetSuccess(false);
        return;
    }

    if (result == NFD_CANCEL)
        return;

    std::filesystem::path localPath = outPath.get();

    XBDM::Console &console = ConsoleStore::GetConsole();

    ConsoleStore::Try([&]() { console.ReceiveFile(LocationMover::GetCurrentConsoleLocation() + '\\' + m_Data.Name, localPath.string()); });
}

void File::Delete()
{
    auto Delete = [&]() {
        XBDM::Console &console = ConsoleStore::GetConsole();

        bool success = ConsoleStore::Try([&]() { console.DeleteFile(LocationMover::GetCurrentConsoleLocation() + '\\' + m_Data.Name, m_Data.IsDirectory); });

        if (success)
            UpdateContents();
    };

    UI::SetConfirmCallback(Delete);
    UI::SetConfirmMessage("Are you sure you want to delete \"" + m_Data.Name + '\"' + (m_Data.IsDirectory ? " and all of its contents" : "") + '?');
    UI::SetConfirm(true);
}

void File::Rename()
{
    auto rename = [&](const std::string &name) {
        XBDM::Console &console = ConsoleStore::GetConsole();
        std::string consoleLocation = LocationMover::GetCurrentConsoleLocation();

        bool success = ConsoleStore::Try([&]() { console.RenameFile(consoleLocation + '\\' + m_Data.Name, consoleLocation + '\\' + name); });

        if (success)
            UpdateContents();
    };

    UI::SetInputTextCallback(rename);
    UI::SetInputTextHeader("Enter a name");
    UI::SetInputTextDefaultValue(m_Data.Name);
    UI::DisplayInputText(true);
}

void File::UpdateContents()
{
    XBDM::Console &console = ConsoleStore::GetConsole();
    std::set<XBDM::File> files;
    std::string consoleLocation = LocationMover::GetCurrentConsoleLocation();

    // If the current location is a drive (e.g hdd:), we need to append '\' to it
    bool success = ConsoleStore::Try([&]() { files = console.GetDirectoryContents(consoleLocation.back() == ':' ? consoleLocation + '\\' : consoleLocation); });

    if (!success)
        return;

    auto fileElements = std::vector<Ref<Element>>();
    fileElements.reserve(files.size());

    for (auto &file : files)
        fileElements.emplace_back(CreateRef<File>(file));

    ContentsChangeEvent event(fileElements);
    m_EventCallback(event);
}

void File::DisplayProperties()
{
    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowSize(ImVec2(380, 400));

    std::string windowTitle = "Properties of " + m_Data.Name;
    ImGui::Begin(windowTitle.c_str(), &m_ShowPropertiesWindow, windowFlags);

    // File name and type
    const char fileNameText[] = "Name:\t";
    const char fileTypeText[] = "Type:\t";
    ImVec2 fileNameTextSize = ImGui::CalcTextSize(fileNameText);
    ImVec2 fileTypeTextSize = ImGui::CalcTextSize(fileTypeText);
    float fileNameAndTypeOffset = std::max<float>(fileNameTextSize.x, fileTypeTextSize.x) + ImGui::GetStyle().ItemSpacing.x * 2.0f;

    std::string fileType;
    if (!m_Data.IsDirectory)
    {
        std::string fileExtension = std::filesystem::path(m_Data.Name).extension().string();
        if (!fileExtension.empty())
        {
            fileExtension = fileExtension.substr(1);
            std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::toupper);
            fileType = fileExtension + " file";
        }
        else
            fileType = "File";
    }
    else
        fileType = "Folder";


    ImGui::TextUnformatted(fileNameText);
    ImGui::SameLine(fileNameAndTypeOffset);
    ImGui::TextUnformatted(m_Data.Name.c_str());
    ImGui::TextUnformatted(fileTypeText);
    ImGui::SameLine(fileNameAndTypeOffset);
    ImGui::TextUnformatted(fileType.c_str());

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();

    // Location and size
    const char locationText[] = "Location:\t";
    const char sizeText[] = "Size:\t";
    ImVec2 locationTextSize = ImGui::CalcTextSize(locationText);
    ImVec2 sizeTextSize = ImGui::CalcTextSize(sizeText);
    float locationAndSizeOffset = std::max<float>(locationTextSize.x, sizeTextSize.x) + ImGui::GetStyle().ItemSpacing.x * 2.0f;

    ImGui::TextUnformatted(locationText);
    ImGui::SameLine(locationAndSizeOffset);
    ImGui::TextUnformatted(LocationMover::GetCurrentConsoleLocation().c_str());
    if (!m_Data.IsDirectory)
    {
        ImGui::TextUnformatted(sizeText);
        ImGui::SameLine(locationAndSizeOffset);
        ImGui::TextUnformatted(NumberFormatter::FileSize(m_Data.Size).c_str());
    }

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();

    // Creation and modification dates
    const char creationDateText[] = "Created:\t";
    const char modificationDateText[] = "Modified:\t";
    ImVec2 creationDateTextSize = ImGui::CalcTextSize(creationDateText);
    ImVec2 modificationDateTextSize = ImGui::CalcTextSize(modificationDateText);
    float datesOffset = std::max<float>(creationDateTextSize.x, modificationDateTextSize.x) + ImGui::GetStyle().ItemSpacing.x * 2.0f;

    char creationDateString[50] = { 0 };
    std::tm *pCreationDateTime = std::localtime(&m_Data.CreationDate);
    std::strftime(creationDateString, sizeof(creationDateString), "%A, %B %d, %Y %H:%M:%S", pCreationDateTime);
    ImGui::TextUnformatted(creationDateText);
    ImGui::SameLine(datesOffset);
    ImGui::TextUnformatted(creationDateString);

    char modificationDateString[50] = { 0 };
    std::tm *pModificationDateTime = std::localtime(&m_Data.ModificationDate);
    std::strftime(modificationDateString, sizeof(modificationDateString), "%A, %B %d, %Y %H:%M:%S", pModificationDateTime);
    ImGui::TextUnformatted(modificationDateText);
    ImGui::SameLine(datesOffset);
    ImGui::TextUnformatted(modificationDateString);

    ImGui::End();
}

void File::DisplayContextMenu()
{
    if (ImGui::BeginPopupContextItem())
    {
        if (!m_Data.IsDirectory)
        {
            if (ImGui::Button("Download"))
            {
                Download();
                ImGui::CloseCurrentPopup();
            }
        }

        if (ImGui::Button("Delete"))
        {
            Delete();
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::Button("Rename"))
        {
            Rename();
            ImGui::CloseCurrentPopup();
        }

        ImGui::Separator();

        if (ImGui::Button("Properties"))
        {
            m_ShowPropertiesWindow = true;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if (m_ShowPropertiesWindow)
        DisplayProperties();
}
