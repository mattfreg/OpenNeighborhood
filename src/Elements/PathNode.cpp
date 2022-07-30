#include "pch.h"
#include "Elements/PathNode.h"

#include "Helpers/ConsoleStore.h"
#include "Helpers/LocationMover.h"
#include "Helpers/ConfigManager.h"
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
    // If m_PosInPath is std::string::npos, then the PathNode is not a directory on the console
    // but the console itself or the App root (OpenNeighborhood)
    if (m_PosInPath == std::string::npos)
    {
        LocationMover::SetCurrentConsoleLocation("\\");

        if (m_Label == ConsoleStore::GetConsole().GetName())
            GoToDrives();
        else if (m_Label == "OpenNeighborhood")
            GoToRoot();

        return;
    }

    std::string newXboxLocation;

    // We start at index 2 because the first 2 PathNodes are "OpenNeighborhood" and the console name
    for (size_t i = 2; i <= m_PosInPath; i++)
    {
        newXboxLocation += m_PathPanel->m_PathNodes[i].GetLabel();

        if (i < m_PosInPath)
            newXboxLocation += '\\';
    }

    XBDM::Console &console = ConsoleStore::GetConsole();
    std::set<XBDM::File> files;

    // If the new location ends with ':', then it's a drive and we need to add '\' at the end
    bool success = ConsoleStore::Try([&]() { files = console.GetDirectoryContents(newXboxLocation.back() == ':' ? newXboxLocation + '\\' : newXboxLocation); });

    if (!success)
        return;

    LocationMover::SetCurrentConsoleLocation(newXboxLocation);

    auto fileElements = std::vector<Ref<Element>>();
    fileElements.reserve(files.size());

    for (auto &file : files)
        fileElements.emplace_back(CreateRef<File>(file));

    ContentsChangeEvent event(fileElements);
    m_EventCallback(event);
};

void PathNode::GoToDrives()
{
    XBDM::Console &console = ConsoleStore::GetConsole();
    std::vector<XBDM::Drive> drives;

    bool success = ConsoleStore::Try([&]() { drives = console.GetDrives(); });

    if (!success)
        return;

    LocationMover::SetCurrentAppLocation(LocationMover::AppLocation::DriveList);

    auto driveElements = std::vector<Ref<Element>>();
    driveElements.reserve(drives.size());

    for (auto &drive : drives)
        driveElements.emplace_back(CreateRef<Drive>(drive));

    ContentsChangeEvent event(driveElements);
    m_EventCallback(event);
}

void PathNode::GoToRoot()
{
    auto elements = std::vector<Ref<Element>>();

    elements.emplace_back(CreateRef<AddXboxButton>());

    ConfigManager::Config config = ConfigManager::GetConfig();

    for (auto &[xboxName, _] : config)
        if (config.get(xboxName).has("ip_address"))
            elements.emplace_back(CreateRef<Xbox>(xboxName, config.get(xboxName).get("ip_address")));

    LocationMover::SetCurrentAppLocation(LocationMover::AppLocation::Root);

    ContentsChangeEvent event(elements);
    m_EventCallback(event);
}
