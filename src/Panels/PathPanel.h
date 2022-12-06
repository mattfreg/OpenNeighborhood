#pragma once

#include "Panels/Panel.h"
#include "Events/AppEvent.h"
#include "Elements/GoToParentButton.h"
#include "Elements/PathNode.h"

class PathPanel : public Panel
{
public:
    friend class PathNode;

    PathPanel();

    virtual void OnRender() override;
    virtual void OnEvent(Event &event) override;

private:
    std::vector<PathNode> m_PathNodes;
    std::queue<ContentsChangeEvent> m_ContentsChangeEventQueue;
    GoToParentButton m_GoToParentButton;

    bool OnCurrentXboxLocationChange(ContentsChangeEvent &event);
    void UpdateDirectories();
};
