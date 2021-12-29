#include "pch.h"
#include "Elements/PathNode.h"

#include "Xbox/XboxManager.h"
#include "Panels/PathPanel.h"
#include "Elements/File.h"
#include "Render/UI.h"


PathNode::PathNode(const std::string &label, size_t posInPath, void *parentPanel)
    : m_PosInPath(posInPath), m_ParentPanel(parentPanel), Element(label, "") {}

void PathNode::OnRender()
{
    if (ImGui::Button(m_Label.c_str()))
        OnClick();
}

void PathNode::OnClick()
{
    std::string newXboxLocation;

    for (size_t i = 0; i <= m_PosInPath; i++)
    {
        newXboxLocation += ((PathPanel *)m_ParentPanel)->m_PathNodes[i].GetLabel();

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
