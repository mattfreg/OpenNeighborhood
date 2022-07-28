#include "pch.h"
#include "Core/App.h"

#include "Core/Assert.h"
#include "Xbox/XboxManager.h"

App *App::s_Instance = nullptr;

App::App(const std::string &name)
{
    // App is a singleton so here we make sure only one instance can live at a time
    ASSERT(!s_Instance, "App already exists!");
    s_Instance = this;

    m_Window = Window::Create(WindowProps(name));
    m_Window->SetEventCallback(BIND_EVENT_FN(App::OnEvent));
}

App::~App()
{
    for (Layer *layer : m_LayerStack)
        layer->OnDetach();
}

void App::PushLayer(Layer *layer)
{
    m_LayerStack.Push(layer);
    layer->OnAttach();
}

void App::Close()
{
    XBDM::Console &xbox = XboxManager::GetConsole();
    if (xbox.IsConnected())
        xbox.CloseConnection();

    m_Running = false;
}

void App::Run()
{
    while (m_Running)
    {
        for (Layer *layer : m_LayerStack)
            layer->OnUpdate();

        m_Window->OnUpdate();
    }
}

void App::OnEvent(Event &event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(App::OnWindowClose));

    // A picture is worth a thousand words.
    // ┌────────────────┐
    // │ Layer 1        │
    // │   ┌────────────────┐
    // │   │  *             │
    // └───│                │
    //     │        Layer 2 │
    //     └────────────────┘
    // If you were to click where the '*' is, you would want to event to be triggered in Layer 2
    // and not Layer 1 even if the click was technically where Layer 1 is.
    // That's why we propagate the events backwards in the layer stack and break out of the loop
    // once the event has been marked as handled (Layer 2 would be at the bottom of the stack here).
    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
    {
        (*--it)->OnEvent(event);
        if (event.IsHandled())
            break;
    }
}

bool App::OnWindowClose(WindowCloseEvent &)
{
    Close();
    return true;
}

bool App::OnWindowResize(WindowResizeEvent &event)
{
    if (event.GetWidth() == 0.0f || event.GetHeight() == 0.0f)
    {
        m_Minimized = true;
        return false;
    }

    m_Minimized = false;

    return false;
}
