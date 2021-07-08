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
    virtual void OnEvent(Event& event) override;

    inline static float GetWidth() { return s_Width; }
    inline static float GetHeight() { return s_Height; }

    static void SetWidth(float width) { s_Width = width; }
    static void SetHeight(float height) { s_Height = height; }
private:
    std::vector<PathNode> m_PathNodes;
    std::queue<ContentsChangeEvent> m_ContentsChangeEventQueue;
    static float s_Width;
    static float s_Height;
    GoToParentButton m_GoToParentButton;

    bool OnCurrentXboxLocationChange(ContentsChangeEvent& event);
    void UpdateDirectories();
};
