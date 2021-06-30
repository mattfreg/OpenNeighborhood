#include "pch.h"
#include "Elements/PathNode.h"

#include "Xbox/XboxManager.h"
#include "Panels/PathPanel.h"
#include "Elements/File.h"
#include "Render/UI.h"

PathNode::PathNode(const std::string& label, size_t posInPath, void* parentPanel)
	: Element(label, "", "Couldn't access directory"), m_PosInPath(posInPath), m_ParentPanel(parentPanel) {}

void PathNode::OnRender()
{
	if (ImGui::Button(m_Label.c_str()))
		OnClick();

	if (!m_Success)
	{
		ImGui::OpenPopup("Error");

		ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	}

	if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::PushFont(UI::GetRegularFont());
		ImGui::Text("%s", m_ErrorMessage.c_str());

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			m_Success = true;
			ImGui::CloseCurrentPopup();
		}

		ImGui::PopFont();
		ImGui::EndPopup();
	}
}

void PathNode::OnClick()
{
	std::string newXboxLocation;

	for (size_t i = 0; i <= m_PosInPath; i++)
	{
		newXboxLocation += ((PathPanel*)m_ParentPanel)->m_PathNodes[i].GetLabel();

		if (i < m_PosInPath)
			newXboxLocation += '\\';
	}

	XBDM::Console& xbox = XboxManager::GetConsole();
	std::vector<XBDM::File> files;

	try
	{
		files = xbox.GetDirectoryContents(newXboxLocation[newXboxLocation.length() - 1] == ':' ? newXboxLocation + '\\' : newXboxLocation);
	}
	catch (const std::invalid_argument&)
	{
		m_Success = false;
	}

	if (m_Success)
	{
		XboxManager::SetCurrentLocation(newXboxLocation);

		Ref<std::vector<Ref<Element>>> fileElements = CreateRef<std::vector<Ref<Element>>>();

		for (auto& file : files)
			fileElements->emplace_back(CreateRef<File>(file));

		ContentsChangeEvent event(fileElements);
		m_EventCallback(event);
	}
};
