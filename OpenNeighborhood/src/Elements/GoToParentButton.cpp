#include "pch.h"
#include "Elements/GoToParentButton.h"

#include "Render/TextureManager.h"
#include "Events/AppEvent.h"

GoToParentButton::GoToParentButton()
	: Element("", "leftArrow", "Couldn't access directory")
{
	auto texture = TextureManager::GetTexture(m_TextureName);
	m_Width = texture->GetWidth();
	m_Height = texture->GetHeight();
}

void GoToParentButton::OnRender()
{
	auto texture = TextureManager::GetTexture(m_TextureName);

	if (ImGui::ImageButton((void*)(intptr_t)texture->GetTextureID(), ImVec2((float)m_Width, (float)m_Height)))
		OnClick();

	if (!m_Success)
	{
		ImGui::OpenPopup("Error");

		ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	}

	if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("%s", m_ErrorMessage.c_str());

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			m_Success = true;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void GoToParentButton::OnClick()
{
	DirectoryChangeEvent event;
	m_EventCallback(event);
}
