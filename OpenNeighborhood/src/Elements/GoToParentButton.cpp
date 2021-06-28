#include "pch.h"
#include "Elements/GoToParentButton.h"

#include "Render/TextureManager.h"
#include "Events/AppEvent.h"
#include "Xbox/XboxManager.h"
#include "Elements/File.h"
#include "Render/UI.h"

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

void GoToParentButton::OnClick()
{
	std::string currentLocation = XboxManager::GetCurrentLocation();
	std::string parentLocation = XboxManager::GetParent();

	if (currentLocation == parentLocation)
		return;

	if (parentLocation[parentLocation.length() - 1] == ':')
		parentLocation += '\\';

	XBDM::Console& xbox = XboxManager::GetConsole();
	std::vector<XBDM::File> files;

	try
	{
		files = xbox.GetDirectoryContents(parentLocation);
	}
	catch (const std::invalid_argument&)
	{
		m_Success = false;
	}

	if (m_Success)
	{
		XboxManager::GoToParent();

		Ref<std::vector<Ref<Element>>> fileElements = CreateRef<std::vector<Ref<Element>>>();

		for (auto& file : files)
			fileElements->emplace_back(CreateRef<File>(file));

		ContentsChangeEvent event(fileElements);
		m_EventCallback(event);
	}
}
