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

    inline float GetWidth() { return m_Width; }

    inline float GetHeight() { return m_Height; }

    inline void SetWidth(float width) { m_Width = width; }

    inline void SetHeight(float height) { m_Height = height; }

private:
    std::vector<PathNode> m_PathNodes;
    std::queue<ContentsChangeEvent> m_ContentsChangeEventQueue;
    float m_Width;
    float m_Height;
    GoToParentButton m_GoToParentButton;

    bool OnCurrentXboxLocationChange(ContentsChangeEvent &event);
    void UpdateDirectories();
};
