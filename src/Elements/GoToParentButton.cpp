#include "pch.h"
#include "Elements/GoToParentButton.h"

#define MINI_CASE_SENSITIVE
#include <mINI/ini.h>

#include "Render/TextureManager.h"
#include "Events/AppEvent.h"
#include "Xbox/XboxManager.h"
#include "Elements/File.h"
#include "Elements/Drive.h"
#include "Elements/AddXboxButton.h"
#include "Elements/Xbox.h"
#include "Render/UI.h"

GoToParentButton::GoToParentButton()
    : Element("", "leftArrow")
{
    auto texture = TextureManager::GetTexture(m_TextureName);
    m_Width = texture->GetWidth();
    m_Height = texture->GetHeight();
}

void GoToParentButton::OnRender()
{
    auto texture = TextureManager::GetTexture(m_TextureName);

    if (ImGui::ImageButton(reinterpret_cast<void *>(static_cast<intptr_t>(texture->GetTextureID())), ImVec2(static_cast<float>(m_Width), static_cast<float>(m_Height))))
        OnClick();
}

void GoToParentButton::OnClick()
{
    const std::string &currentLocation = XboxManager::GetCurrentLocation();
    std::string parentLocation = XboxManager::GetParent();

    if (parentLocation == "\\")
    {
        if (currentLocation != "\\")
            XboxManager::GoToParent();

        XboxManager::Position currentPosition = XboxManager::GetCurrentPosition();
        switch (currentPosition)
        {
        case XboxManager::Position::DriveContents:
            GoToDrives();
            break;
        case XboxManager::Position::DriveList:
            GoToRoot();
            break;
        }

        return;
    }

    if (parentLocation.back() == ':')
        parentLocation += '\\';

    XBDM::Console &xbox = XboxManager::GetConsole();
    std::set<XBDM::File> files;

    bool success = XboxManager::Try([&]() { files = xbox.GetDirectoryContents(parentLocation); });

    if (!success)
        return;

    XboxManager::GoToParent();

    auto fileElements = std::vector<Ref<Element>>();

    for (auto &file : files)
        fileElements.emplace_back(CreateRef<File>(file));

    ContentsChangeEvent event(fileElements);
    m_EventCallback(event);
}

void GoToParentButton::GoToDrives()
{
    XBDM::Console &xbox = XboxManager::GetConsole();
    std::vector<XBDM::Drive> drives;

    bool success = XboxManager::Try([&]() { drives = xbox.GetDrives(); });

    if (!success)
        return;

    XboxManager::SetCurrentPosition(XboxManager::Position::DriveList);

    auto driveElements = std::vector<Ref<Element>>();

    for (auto &drive : drives)
        driveElements.emplace_back(CreateRef<Drive>(drive));

    ContentsChangeEvent event(driveElements);
    m_EventCallback(event);
}

void GoToParentButton::GoToRoot()
{
    auto elements = std::vector<Ref<Element>>();

    elements.emplace_back(CreateRef<AddXboxButton>());

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
                elements.emplace_back(CreateRef<Xbox>(consoleName, ipAddress));
            }
        }
    }

    XboxManager::SetCurrentPosition(XboxManager::Position::Root);

    ContentsChangeEvent event(elements);
    m_EventCallback(event);
}
