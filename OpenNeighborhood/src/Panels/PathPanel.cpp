#include "pch.h"
#include "Panels/PathPanel.h"

#include "Xbox/XboxManager.h"

void PathPanel::OnRender()
{
	ImGuiWindowFlags windowFlags =
		  ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus
		| ImGuiWindowFlags_NoNavFocus
	;

	ImGui::SetNextWindowPos(ImVec2(m_Margin, m_Margin));
	ImGui::SetNextWindowSize(ImVec2((float)m_WindowWidth - m_Margin * 2.0f, m_Margin * 5.0f));

	ImGui::Begin("Path Window", nullptr, windowFlags);
	ImGui::End();
}

void PathPanel::OnEvent(Event& event)
{
	Panel::OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<DirectoryChangeEvent>(BIND_EVENT_FN(PathPanel::OnCurrentXboxLocationChange));
}

bool PathPanel::OnCurrentXboxLocationChange(DirectoryChangeEvent& event)
{
	m_Directories.clear();
	std::string locationCopy = XboxManager::GetCurrentLocation() + '\\';
	size_t pos = 0;

	while ((pos = locationCopy.find('\\')) != std::string::npos)
	{
		std::string directory = locationCopy.substr(0, pos);
		m_Directories.push_back(directory);

		locationCopy.erase(0, pos + 1);
	}

	for (auto& directory : m_Directories)
		LOG_INFO(directory);

	return true;
}
