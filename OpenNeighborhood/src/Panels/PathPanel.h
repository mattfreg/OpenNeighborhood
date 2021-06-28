#pragma once

#include "Panels/Panel.h"
#include "Events/AppEvent.h"
#include "Elements/GoToParentButton.h"

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
	std::queue<ContentsChangeEvent> m_ContentsChangeEventQueue;
	static float s_Width;
	static float s_Height;
	GoToParentButton m_GoToParentButton;

	bool OnCurrentXboxLocationChange(ContentsChangeEvent& event);
	void UpdateDirectories();
};
