#pragma once

#include "Elements/Element.h"

class GoToParentButton : public Element
{
public:
	GoToParentButton();

	virtual void OnRender() override;
	virtual void OnClick() override;
};
