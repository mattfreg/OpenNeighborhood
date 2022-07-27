#include "pch.h"
#include "Elements/PathNode.h"

#define MINI_CASE_SENSITIVE
#include <mINI/ini.h>

#include "Xbox/XboxManager.h"
#include "Panels/PathPanel.h"
#include "Elements/File.h"
#include "Elements/Drive.h"
#include "Elements/AddXboxButton.h"
#include "Elements/Xbox.h"
#include "Render/UI.h"

PathNode::PathNode(const std::string &label, size_t posInPath, PathPanel *pathPanel)
    : m_PosInPath(posInPath), m_PathPanel(pathPanel), Element(label, "")
{
}

void PathNode::OnRender()
{
    if (ImGui::Button(m_Label.c_str()))
        OnClick();
}

void PathNode::OnClick()
{
    if (m_PosInPath == std::string::npos)
    {
        XboxManager::SetCurrentLocation("\\");

        if (m_Label == XboxManager::GetConsole().GetName())
            GoToDrives();
        else if (m_Label == "OpenNeighborhood")
            GoToRoot();

        return;
    }

    std::string newXboxLocation;

    for (size_t i = 2; i <= m_PosInPath; i++)
    {
        newXboxLocation += m_PathPanel->m_PathNodes[i].GetLabel();

        if (i < m_PosInPath)
            newXboxLocation += '\\';
    }

    XBDM::Console &xbox = XboxManager::GetConsole();
    std::set<XBDM::File> files;

    // If the new location ends with ':', then it's a drive and we need to add '\' at the end
    bool success = XboxManager::Try([&]() { files = xbox.GetDirectoryContents(newXboxLocation.back() == ':' ? newXboxLocation + '\\' : newXboxLocation); });

    if (!success)
        return;

    XboxManager::SetCurrentLocation(newXboxLocation);

    auto fileElements = std::vector<Ref<Element>>();

    for (auto &file : files)
        fileElements.emplace_back(CreateRef<File>(file));

    ContentsChangeEvent event(fileElements);
    m_EventCallback(event);
};

void PathNode::GoToDrives()
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

void PathNode::GoToRoot()
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
