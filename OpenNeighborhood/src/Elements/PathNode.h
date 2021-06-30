#pragma once

#include "Elements/Element.h"

class PathNode : public Element
{
public:
	PathNode(const std::string& label, size_t posInPath, void* parentPanel);

	virtual void OnRender() override;
	virtual void OnClick() override;
private:
	size_t m_PosInPath;
	void* m_ParentPanel;
};
