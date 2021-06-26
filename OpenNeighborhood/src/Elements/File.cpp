#include "pch.h"
#include "Elements/File.h"

#include "Xbox/XboxManager.h"
#include "Events/AppEvent.h"

File::File(const XBDM::File& data)
	: m_Data(data), Element(data.Name, data.IsDirectory ? "directory" : data.IsXEX ? "xex" : "file", "Couldn't access file!") {}

void File::OnClick()
{
	if (m_Data.IsDirectory)
		OpenDirectory();
	else if (m_Data.IsXEX)
		LaunchXEX();
}

void File::OpenDirectory()
{
	XBDM::Console xbox = XboxManager::GetConsole();
	std::vector<XBDM::File> files;

	try
	{
		files = xbox.GetDirectoryContents(XboxManager::GetCurrentLocation() + '\\' + m_Data.Name);
	}
	catch (const std::invalid_argument& exception)
	{
		m_Success = false;
	}

	if (m_Success)
	{
		Ref<std::vector<Ref<Element>>> fileElements = CreateRef<std::vector<Ref<Element>>>();

		for (auto& file : files)
			fileElements->emplace_back(CreateRef<File>(file));

		ContentsChangeEvent contentsChangeEvent(fileElements);
		DirectoryChangeEvent directoryChangeEvent;
		m_EventCallback(contentsChangeEvent);
		m_EventCallback(directoryChangeEvent);

		XboxManager::GoToDirectory(m_Data.Name);
	}
}

void File::LaunchXEX()
{
	XBDM::Console xbox = XboxManager::GetConsole();
	xbox.LaunchXEX(XboxManager::GetCurrentLocation() + '\\' + m_Data.Name);
}
