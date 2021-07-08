#pragma once

#include <GLFW/glfw3.h>

#include "Events/Event.h"

struct WindowProps
{
    std::string Title;
    float Width;
    float Height;

    WindowProps(const std::string& title = "OpenNeighborhood",
        float width = 1280,
        float height = 720)
        : Title(title), Width(width), Height(height) {}
};

class Window
{
public:
    using EventCallbackFn = std::function<void(Event&)>;

    Window(const WindowProps& props);
    virtual ~Window();

    void OnUpdate();

    inline float GetWidth() const { return m_Data.Width; }
    inline float GetHeight() const { return m_Data.Height; }

    // Window attributes
    inline void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
    void SetVSync(bool enabled);
    bool IsVSync() const { return m_Data.VSync; }

    inline void* GetNativeWindow() const { return m_Window; }

    static Window* Create(const WindowProps& props = WindowProps());
private:
    GLFWwindow* m_Window;

    struct WindowData
    {
        std::string Title;
        float Width;
        float Height;
        bool VSync;

        EventCallbackFn EventCallback;
    };

    WindowData m_Data;

    void Init(const WindowProps& props);
    void Shutdown();
};
