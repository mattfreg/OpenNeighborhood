#include "pch.h"
#include "Panels/PathPanel.h"

#include "Xbox/XboxManager.h"
#include "Render/UI.h"
#include "Render/TextureManager.h"

float PathPanel::s_Width;
float PathPanel::s_Height;

PathPanel::PathPanel()
{
	std::string textureName = "leftArrow";

	if (!TextureManager::TextureExists(textureName))
		TextureManager::AddTexture(textureName, WORKING_DIR"assets/icons/" + textureName + ".png");

	s_Width = (float)m_WindowWidth - m_Margin * 2.0f;
	s_Height = m_Margin * 2.4f;
}

void PathPanel::OnRender()
{
	ImGuiWindowFlags windowFlags =
		  ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus
		| ImGuiWindowFlags_NoNavFocus
		| ImGuiWindowFlags_HorizontalScrollbar
	;

	s_Width = (float)m_WindowWidth - m_Margin * 2.0f;

	ImGui::SetNextWindowPos(ImVec2(m_Margin, m_Margin));
	ImGui::SetNextWindowSize(ImVec2(s_Width, s_Height));

	ImGui::Begin("Path Window", nullptr, windowFlags);
	ImGui::PushFont(UI::GetRegularBigFont());

	RenderGoToParentButton();
	ImGui::SameLine();
	ImGui::Text("%s", "|");
	ImGui::SameLine();

	for (size_t i = 0; i < m_Directories.size(); i++)
	{
		ImGui::PushID((int)i);

		if (ImGui::Button(m_Directories[i].c_str()))
			LOG_INFO(m_Directories[i] + " clicked");

		if (i + 1 < m_Directories.size())
		{
			ImGui::SameLine();
			ImGui::Text("%s", ">");
			ImGui::SameLine();
		}

		ImGui::PopID();
	}

	ImGui::PopFont();
	ImGui::End();

	if (!m_DirectoryChangeEventQueue.empty())
		UpdateDirectories();
}

void PathPanel::OnEvent(Event& event)
{
	Panel::OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<DirectoryChangeEvent>(BIND_EVENT_FN(PathPanel::OnCurrentXboxLocationChange));
}

bool PathPanel::OnCurrentXboxLocationChange(DirectoryChangeEvent& event)
{
	m_DirectoryChangeEventQueue.push(event);

	return true;
}

void PathPanel::UpdateDirectories()
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

	m_DirectoryChangeEventQueue.pop();
}

void PathPanel::RenderGoToParentButton()
{
	auto texture = TextureManager::GetTexture("leftArrow");
	uint32_t width = texture->GetWidth() * 3;
	uint32_t height = texture->GetHeight();

	if (ImGui::ImageButton((void*)(intptr_t)texture->GetTextureID(), ImVec2((float)texture->GetWidth(), (float)texture->GetHeight())))
		LOG_INFO("GoToParent button clicked");
}
