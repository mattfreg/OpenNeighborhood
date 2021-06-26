#pragma once

#include "Panels/Panel.h"
#include "Events/AppEvent.h"

class PathPanel : public Panel
{
public:
	virtual void OnRender() override;
	virtual void OnEvent(Event& event) override;
private:
	std::vector<std::string> m_Directories;
	std::queue<DirectoryChangeEvent> m_DirectoryChangeEventQueue;

	bool OnCurrentXboxLocationChange(DirectoryChangeEvent& event);
	void UpdateDirectories();
};
