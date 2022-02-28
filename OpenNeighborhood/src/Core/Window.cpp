#include "pch.h"
#include "Core/Window.h"

#include <glad/glad.h>
#include <nfd.hpp>

#include "Events/AppEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include "Core/Log.h"
#include "Core/Assert.h"


static bool s_GLFWInitialized = false;

static void GLFWErrorCallback(int error, const char *desc)
{
    Log::Error("GLFW error code: (%i) | %s", error, desc);
}

Window *Window::Create(const WindowProps &props)
{
    return new Window(props);
}

Window::Window(const WindowProps &props)
{
    Init(props);
}

Window::~Window()
{
    Shutdown();
}

void Window::Init(const WindowProps &props)
{
    m_Data.Title = props.Title;
    m_Data.Width = props.Width;
    m_Data.Height = props.Height;

    if (!s_GLFWInitialized)
    {
        int success = glfwInit();
        ASSERT(success, "Could not initialize GLFW!");

        NFD::Guard nfdGuard;

        glfwSetErrorCallback(GLFWErrorCallback);

        s_GLFWInitialized = true;
    }

    m_Window = glfwCreateWindow(static_cast<int>(props.Width), static_cast<int>(props.Height), m_Data.Title.c_str(), nullptr, nullptr);

    glfwMakeContextCurrent(m_Window);

    int status = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
    ASSERT(status, "Failed to initialize Glad!");

    glfwSetWindowUserPointer(m_Window, &m_Data);
    SetVSync(true);

    // Set GLFW callbacks
    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *window, int width, int height)
        {
            WindowData &windowData = *reinterpret_cast<WindowData *>(glfwGetWindowUserPointer(window));
            windowData.Width = static_cast<float>(width);
            windowData.Height = static_cast<float>(height);

            WindowResizeEvent event(static_cast<float>(width), static_cast<float>(height));
            windowData.EventCallback(event);
        });

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window)
        {
            WindowData &windowData = *reinterpret_cast<WindowData *>(glfwGetWindowUserPointer(window));
            WindowCloseEvent event;
            windowData.EventCallback(event);
        });

    glfwSetKeyCallback(m_Window, [](GLFWwindow *window, int key, int scancode, int action, int mods)
        {
            WindowData &windowData = *reinterpret_cast<WindowData *>(glfwGetWindowUserPointer(window));

            switch (action)
            {
            case GLFW_PRESS:
            {
                KeyPressedEvent event(key, 0);
                windowData.EventCallback(event);
                break;
            }
            case GLFW_RELEASE:
            {
                KeyReleasedEvent event(key);
                windowData.EventCallback(event);
                break;
            }
            case GLFW_REPEAT:
            {
                KeyPressedEvent event(key, 1);
                windowData.EventCallback(event);
                break;
            }
            }
        });

    glfwSetCharCallback(m_Window, [](GLFWwindow *window, unsigned int keyCode)
        {
            WindowData &windowData = *reinterpret_cast<WindowData *>(glfwGetWindowUserPointer(window));

            KeyTypedEvent event(keyCode);
            windowData.EventCallback(event);
        });

    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button, int action, int mods)
        {
            WindowData &windowData = *reinterpret_cast<WindowData *>(glfwGetWindowUserPointer(window));

            switch (action)
            {
            case GLFW_PRESS:
            {
                MouseButtonPressedEvent event(button);
                windowData.EventCallback(event);
                break;
            }
            case GLFW_RELEASE:
            {
                MouseButtonReleasedEvent event(button);
                windowData.EventCallback(event);
                break;
            }
            }
        });

    glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double xOffset, double yOffset)
        {
            WindowData &windowData = *reinterpret_cast<WindowData *>(glfwGetWindowUserPointer(window));

            MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
            windowData.EventCallback(event);
        });

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double x, double y)
        {
            WindowData &windowData = *reinterpret_cast<WindowData *>(glfwGetWindowUserPointer(window));

            MouseMovedEvent event(static_cast<float>(x), static_cast<float>(y));
            windowData.EventCallback(event);
        });
}

void Window::Shutdown()
{
    glfwDestroyWindow(m_Window);
}

void Window::OnUpdate()
{
    glfwPollEvents();
    glfwSwapBuffers(m_Window);
}

void Window::SetVSync(bool enabled)
{
    if (enabled)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);

    m_Data.VSync = enabled;
}
