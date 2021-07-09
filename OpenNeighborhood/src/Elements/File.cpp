#include "pch.h"
#include "Elements/File.h"

#include <nfd.hpp>

#include "Xbox/XboxManager.h"
#include "Events/AppEvent.h"
#include "Render/UI.h"

File::File(const XBDM::File& data)
    : m_Data(data), Element(data.Name, data.IsDirectory ? "directory" : data.IsXEX ? "xex" : "file") {}

void File::OnClick()
{
    if (m_Data.IsDirectory)
        OpenDirectory();
    else if (m_Data.IsXEX)
        LaunchXEX();
}

void File::OpenDirectory()
{
    XBDM::Console& xbox = XboxManager::GetConsole();
    std::set<XBDM::File> files;

    bool success = XboxManager::Try([&]() { files = xbox.GetDirectoryContents(XboxManager::GetCurrentLocation() + '\\' + m_Data.Name); });

    if (!success)
        return;

    XboxManager::GoToDirectory(m_Data.Name);

    auto fileElements = CreateRef<std::vector<Ref<Element>>>();

    for (auto& file : files)
        fileElements->emplace_back(CreateRef<File>(file));

    ContentsChangeEvent event(fileElements);
    m_EventCallback(event);
}

void File::LaunchXEX()
{
    XBDM::Console& xbox = XboxManager::GetConsole();
    xbox.LaunchXEX(XboxManager::GetCurrentLocation() + '\\' + m_Data.Name);
}

void File::Download()
{
    /**
     * Depending on the system, std::filesystem::path::native can return either
     * std::wstring or std::string. Since we don't know, we are just using auto.
     */
    auto extension = std::filesystem::path(m_Data.Name).extension().native().substr(1);
    auto filterName = extension;
    std::transform(filterName.begin(), filterName.end(), filterName.begin(), [](auto c) { return std::toupper(c); });

    NFD::UniquePathN outPath;
    nfdnfilteritem_t filterItem[] = { { filterName.c_str(), extension.c_str() } };
    nfdresult_t result = NFD::SaveDialog(outPath, filterItem, 1);

    if (result == NFD_ERROR)
    {
        UI::SetErrorMessage(NFD::GetError());
        UI::SetSuccess(false);
        return;
    }
    
    if (result == NFD_CANCEL)
        return;

    std::filesystem::path localPath = outPath.get();

    XBDM::Console& xbox = XboxManager::GetConsole();

    XboxManager::Try([&]() { xbox.ReceiveFile(XboxManager::GetCurrentLocation() + '\\' + m_Data.Name, localPath.string()); });
}

void File::Delete()
{
    auto Delete = [this]()
    {
        XBDM::Console& xbox = XboxManager::GetConsole();

        bool success = XboxManager::Try([&]() { xbox.DeleteFile(XboxManager::GetCurrentLocation() + '\\' + m_Data.Name, m_Data.IsDirectory); });

        if (!success)
            return;

        std::set<XBDM::File> files;
        std::string location = XboxManager::GetCurrentLocation();

        // If the current location is a drive (e.g hdd:), we need to append '\' to it
        location = location.back() == ':' ? location + '\\' : location;

        success = XboxManager::Try([&]() { files = xbox.GetDirectoryContents(location); });

        if (!success)
            return;

        auto fileElements = CreateRef<std::vector<Ref<Element>>>();

        for (auto& file : files)
            fileElements->emplace_back(CreateRef<File>(file));

        ContentsChangeEvent event(fileElements);
        m_EventCallback(event);
    };

    UI::SetConfirmCallback(Delete);
    UI::SetConfirmMessage("Are you sure you want to delete \"" + m_Data.Name + '\"' + (m_Data.IsDirectory ? " and all of its contents" : "") + '?');
    UI::SetConfirm(true);
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

        ImGui::EndPopup();
    }
}
