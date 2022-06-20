#pragma once

#include "Core/Layer.h"
#include "Core/Stack.h"
#include "Panels/Panel.h"


class OpenNeighborhood : public Layer
{
public:
    OpenNeighborhood();
    virtual ~OpenNeighborhood() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate() override;
    virtual void OnEvent(Event &event) override;

    inline static OpenNeighborhood &Get() { return *s_Instance; }
private:
    Stack<Panel *> m_PanelStack;

    static OpenNeighborhood *s_Instance;
};
