#pragma once

#include "Elements/Element.h"

class PathPanel;

class PathNode : public Element
{
public:
    PathNode(const std::string &label, size_t posInPath, PathPanel &pathPanel);

    virtual void OnRender() override;
    virtual void OnClick() override;

private:
    size_t m_PosInPath;
    PathPanel &m_PathPanel;
};
