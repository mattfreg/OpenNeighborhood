#pragma once

#include "Panels/Panel.h"
#include "Events/AppEvent.h"

class PathPanel : public Panel
{
public:
	PathPanel();

	virtual void OnRender() override;
	virtual void OnEvent(Event& event) override;

	inline static float GetWidth() { return s_Width; }
	inline static float GetHeight() { return s_Height; }

	static void SetWidth(float width) { s_Width = width; }
	static void SetHeight(float height) { s_Height = height; }
private:
	std::vector<std::string> m_Directories;
	std::queue<DirectoryChangeEvent> m_DirectoryChangeEventQueue;
	static float s_Width;
	static float s_Height;

	bool OnCurrentXboxLocationChange(DirectoryChangeEvent& event);
	void UpdateDirectories();

	void RenderGoToParentButton();
};
