#include "pch.h"
#include "Elements/File.h"

#include <nfd.hpp>

#include "Core/PlatformDetector.h"
#include "Helpers/ConsoleStore.h"
#include "Helpers/LocationMover.h"
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

        ImGui::EndPopup();
    }
}
