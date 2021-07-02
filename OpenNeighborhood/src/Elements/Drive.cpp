#include "pch.h"
#include "Elements/Drive.h"

#include "Xbox/XboxManager.h"
#include "Events/AppEvent.h"
#include "Elements/File.h"

Drive::Drive(const XBDM::Drive& data)
	: m_Data(data), Element(data.FriendlyName, "drive", "Couldn't fetch content from this drive!") {}

void Drive::OnClick()
{
	std::transform(m_Data.Name.begin(), m_Data.Name.end(), m_Data.Name.begin(), [](unsigned char c) { return std::tolower(c); });

	XBDM::Console& xbox = XboxManager::GetConsole();
	std::vector<XBDM::File> files;

	try
	{
		files = xbox.GetDirectoryContents(XboxManager::GetCurrentLocation() + m_Data.Name + ":\\");
	}
	catch (const std::exception& exception)
	{
		m_ErrorMessage = exception.what();
		m_Success = false;
	}

	if (!m_Success)
		return;

	XboxManager::GoToDirectory(m_Data.Name);

	auto fileElements = CreateRef<std::vector<Ref<Element>>>();

	for (auto& file : files)
		fileElements->emplace_back(CreateRef<File>(file));

	ContentsChangeEvent event(fileElements);
	m_EventCallback(event);
}
