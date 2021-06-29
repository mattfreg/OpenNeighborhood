#include "pch.h"
#include "Elements/PathNode.h"

PathNode::PathNode(const std::string& label)
	: Element(label, "", "Couldn't access directory") {}

void PathNode::OnRender()
{
	if (ImGui::Button(m_Label.c_str()))
		OnClick();
}

void PathNode::OnClick()
{
	LOG_INFO(m_Label + " clicked");
}
