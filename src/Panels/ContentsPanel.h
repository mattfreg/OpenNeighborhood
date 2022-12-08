#pragma once

#include "Panels/Panel.h"
#include "Elements/Element.h"

class ContentsPanel : public Panel
{
public:
    using ConfirmCallbackFn = std::function<void()>;

    ContentsPanel();

    virtual void OnRender() override;
    virtual void OnEvent(Event &event) override;

private:
    std::vector<Ref<Element>> m_Elements;
    std::queue<ContentsChangeEvent> m_ContentsChangeEventQueue;

    bool OnContentsChange(ContentsChangeEvent &event);
    void InjectNewElements();
    void DisplayContextMenu();
    void UpdateContents();

    void Paste();
    void Upload();
    void CreateDirectory();
};
