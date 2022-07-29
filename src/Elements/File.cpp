#include "pch.h"
#include "Elements/File.h"

#include <nfd.hpp>

#include "Xbox/XboxManager.h"
#include "Events/AppEvent.h"
#include "Render/UI.h"

File::File(const XBDM::File &data)
    : m_Data(data), Element(data.Name, data.IsDirectory ? "directory" : data.IsXex ? "xex"
                                                                                   : "file")
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
    XBDM::Console &xbox = XboxManager::GetConsole();
    std::set<XBDM::File> files;

    bool success = XboxManager::Try([&]() { files = xbox.GetDirectoryContents(XboxManager::GetCurrentLocation() + '\\' + m_Data.Name); });

    if (!success)
        return;

    XboxManager::GoToDirectory(m_Data.Name);

    auto fileElements = std::vector<Ref<Element>>();
    fileElements.reserve(files.size());

    for (auto &file : files)
        fileElements.emplace_back(CreateRef<File>(file));

    ContentsChangeEvent event(fileElements);
    m_EventCallback(event);
}

void File::LaunchXex()
{
    XBDM::Console &xbox = XboxManager::GetConsole();
    xbox.LaunchXex(XboxManager::GetCurrentLocation() + '\\' + m_Data.Name);
}

void File::Download()
{
    // Depending on the system, std::filesystem::path::native can return either
    // std::wstring or std::string. Since we don't know, we are just using auto.
    auto filename = std::filesystem::path(m_Data.Name);
    auto extension = filename.extension().native().substr(1);
    auto &filterName = extension;

    // Convert the filter name to lower case
    std::transform(filterName.begin(), filterName.end(), filterName.begin(), [](auto c) { return std::toupper(c); });

    // Oven the save dialog that will get a path to where to save the current file
    NFD::UniquePathN outPath;
    nfdnfilteritem_t filterItem[] = { { filterName.c_str(), extension.c_str() } };
    nfdresult_t result = NFD::SaveDialog(outPath, filterItem, 1, nullptr, filename.native().c_str());

    if (result == NFD_ERROR)
    {
        UI::SetErrorMessage(NFD::GetError());
        UI::SetSuccess(false);
        return;
    }

    if (result == NFD_CANCEL)
        return;

    std::filesystem::path localPath = outPath.get();

    XBDM::Console &xbox = XboxManager::GetConsole();

    XboxManager::Try([&]() { xbox.ReceiveFile(XboxManager::GetCurrentLocation() + '\\' + m_Data.Name, localPath.string()); });
}

void File::Delete()
{
    auto Delete = [&]() {
        XBDM::Console &xbox = XboxManager::GetConsole();

        bool success = XboxManager::Try([&]() { xbox.DeleteFile(XboxManager::GetCurrentLocation() + '\\' + m_Data.Name, m_Data.IsDirectory); });

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
        XBDM::Console &xbox = XboxManager::GetConsole();
        const std::string &location = XboxManager::GetCurrentLocation();

        bool success = XboxManager::Try([&]() { xbox.RenameFile(location + '\\' + m_Data.Name, location + '\\' + name); });

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
    XBDM::Console &xbox = XboxManager::GetConsole();
    std::set<XBDM::File> files;
    const std::string &location = XboxManager::GetCurrentLocation();

    // If the current location is a drive (e.g hdd:), we need to append '\' to it
    bool success = XboxManager::Try([&]() { files = xbox.GetDirectoryContents(location.back() == ':' ? location + '\\' : location); });

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
