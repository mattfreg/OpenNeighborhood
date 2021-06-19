#pragma once

#include <XBDM.h>

class XboxManager
{
public:
	static bool CreateConsole(const std::string& ipAddress, std::string& consoleName, bool keepConnectionOpen = false);

	inline static const XBDM::Console& GetConsole() { return s_Console; }
	inline static const std::string& GetCurrentLocation() { return s_CurrentLocation; }

	static const std::string& GoToDirectory(const std::string& directory);
	static const std::string& GoToParent();
private:
	static XBDM::Console s_Console;
	static std::string s_CurrentLocation;
};
