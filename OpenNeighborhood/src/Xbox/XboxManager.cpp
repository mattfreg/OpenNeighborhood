#include "pch.h"
#include "Xbox/XboxManager.h"

XBDM::Console XboxManager::s_Console;
std::string XboxManager::s_CurrentLocation;

bool XboxManager::CreateConsole(const std::string& ipAddress, std::string& consoleName, bool keepConnectionOpen)
{
	s_Console = XBDM::Console(ipAddress);

	if (!s_Console.OpenConnection())
		return false;

	try
	{
		consoleName = s_Console.GetName();
	}
	catch (const std::exception&)
	{
		s_Console.CloseConnection();
		return false;
	}

	if (!keepConnectionOpen)
		s_Console.CloseConnection();

	return true;
}

const std::string& XboxManager::GoToDirectory(const std::string& directory)
{
	if (directory.front() != '\\' && !s_CurrentLocation.empty())
		s_CurrentLocation += '\\';

	if (s_CurrentLocation.empty())
		s_CurrentLocation += (directory + ':');
	else
		s_CurrentLocation += directory;

	return s_CurrentLocation;
}

std::string XboxManager::GetParent()
{
	std::string path = s_CurrentLocation;
	size_t lastSeparatorIndex = path.find_last_of('\\');

	if (lastSeparatorIndex != std::string::npos)
		path = path.substr(0, lastSeparatorIndex);

	return path;
}

const std::string& XboxManager::GoToParent()
{
	s_CurrentLocation = GetParent();

	return s_CurrentLocation;
}
