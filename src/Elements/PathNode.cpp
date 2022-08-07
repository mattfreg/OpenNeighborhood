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
    : Element(label, ""), m_PosInPath(posInPath), m_PathPanel(pathPanel)
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

        std::vector<Ref<Element>> elements;

        if (m_Label == ConsoleStore::GetConsole().GetName())
            elements = UI::CreateDriveElements();
        else if (m_Label == "OpenNeighborhood")
            elements = UI::CreateRootElements();

        if (elements.empty())
            return;

        ContentsChangeEvent event(elements);
        m_EventCallback(event);

        return;
    }

    std::string newConsoleLocation;

    // We start at index 2 because the first 2 PathNodes are "OpenNeighborhood" and the console name
    for (size_t i = 2; i <= m_PosInPath; i++)
    {
        newConsoleLocation += m_PathPanel->m_PathNodes[i].GetLabel();

        if (i < m_PosInPath)
            newConsoleLocation += '\\';
    }

    XBDM::Console &console = ConsoleStore::GetConsole();
    std::set<XBDM::File> files;

    // If the new location ends with ':', then it's a drive and we need to add '\' at the end
    bool success = ConsoleStore::Try([&]() { files = console.GetDirectoryContents(newConsoleLocation.back() == ':' ? newConsoleLocation + '\\' : newConsoleLocation); });

    if (!success)
        return;

    LocationMover::SetCurrentConsoleLocation(newConsoleLocation);

    auto fileElements = std::vector<Ref<Element>>();
    fileElements.reserve(files.size());

    for (auto &file : files)
        fileElements.emplace_back(CreateRef<File>(file));

    ContentsChangeEvent event(fileElements);
    m_EventCallback(event);
};
