#pragma once

#include "Render/ImGuiCustomWidgets.h"
#include "Events/Event.h"


class Element
{
public:
    using EventCallbackFn = std::function<void(Event &)>;

    Element(const std::string &label, const std::string &textureName);

    virtual ~Element() = default;

    virtual void OnRender();
    virtual void OnClick() = 0;

    inline const std::string &GetLabel() const { return m_Label; }
    inline const std::string &GetTextureName() const { return m_TextureName; }
    inline float GetWidth() const { return m_Width; }
    inline float GetHeight() const { return m_Height; }
protected:
    std::string m_Label;
    std::string m_TextureName;
    float m_Width;
    float m_Height;
    float m_Padding = 10.0f;
    EventCallbackFn m_EventCallback;

    void OnEvent(Event &event);
    virtual void DisplayContextMenu() {}
};
