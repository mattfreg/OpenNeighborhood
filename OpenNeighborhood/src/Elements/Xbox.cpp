#include "pch.h"
#include "Elements/Xbox.h"

#include "Xbox/XboxManager.h"
#include "Events/AppEvent.h"
#include "Elements/Drive.h"

#include "Core/Log.h"

Xbox::Xbox(const std::string& label, const std::string& ipAddress)
	: m_IpAddress(ipAddress), Element(label, "xbox", "Couldn't find console!") {}

void Xbox::OnClick()
{
	std::string consoleName;
	m_Success = XboxManager::CreateConsole(m_IpAddress, consoleName, true);

	if (!m_Success)
		return;

	XBDM::Console& xbox = XboxManager::GetConsole();
	std::vector<XBDM::Drive> drives;

	try
	{
		drives = xbox.GetDrives();
	}
	catch (const std::exception& exception)
	{
		m_ErrorMessage = exception.what();
		m_Success = false;
	}

	if (!m_Success)
		return;

	auto driveElements = CreateRef<std::vector<Ref<Element>>>();

	for (auto& drive : drives)
		driveElements->emplace_back(CreateRef<Drive>(drive));

	ContentsChangeEvent event(driveElements);
	m_EventCallback(event);
}
