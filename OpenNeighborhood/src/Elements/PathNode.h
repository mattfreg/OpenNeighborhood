#pragma once

#include "Elements/Element.h"

class PathNode : public Element
{
public:
	PathNode(const std::string& label);

	virtual void OnRender() override;
	virtual void OnClick() override;
};
