#pragma once

#include <ImGui/imgui.h>
#include <ImGui/imgui_custom_widgets.h>

#include "Events/Event.h"

class Element
{
public:
    using EventCallbackFn = std::function<void(Event&)>;
    using ConfirmCallbackFn = std::function<void()>;

    Element(const std::string& label, const std::string& textureName, const std::string& errorMessage);

    virtual ~Element() = default;

    virtual void OnRender();
    virtual void OnClick() = 0;

    inline const std::string& GetLabel() const { return m_Label; }
    inline const std::string& GetTextureName() const { return m_TextureName; }
    inline float GetWidth() const { return m_Width; }
    inline float GetHeight() const { return m_Height; }
protected:
    std::string m_Label;
    std::string m_TextureName;
    float m_Width;
    float m_Height;
    float m_Padding = 10.0f;
    EventCallbackFn m_EventCallback;

    void OnEvent(Event& event);
    void DisplayErrorModal();
    void DisplayConfirmModal();
    virtual void DisplayContextMenu() {}

    // Confirmation members
    bool m_Confirm = false;
    std::string m_ConfirmMessage;
    ConfirmCallbackFn m_ConfirmCallback;

    // XBDM related members
    bool m_Success = true; // Gets set to false whenever an XBDM operation related to the element fails
    std::string m_ErrorMessage; // Gets printed in a modal window whenever an XBDM operation related to the element fails
};
