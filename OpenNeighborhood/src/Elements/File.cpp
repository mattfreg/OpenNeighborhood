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
	XBDM::Console& xbox = XboxManager::GetConsole();
	std::set<XBDM::File> files;

	try
	{
		files = xbox.GetDirectoryContents(XboxManager::GetCurrentLocation() + '\\' + m_Data.Name);
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

void File::LaunchXEX()
{
	XBDM::Console& xbox = XboxManager::GetConsole();
	xbox.LaunchXEX(XboxManager::GetCurrentLocation() + '\\' + m_Data.Name);
}

void File::Download()
{
	if (m_Data.IsDirectory)
	{
		m_ErrorMessage = "Cannot download a directory";
		m_Success = false;
		return;
	}

	XBDM::Console& xbox = XboxManager::GetConsole();

	try
	{
		xbox.ReceiveFile(XboxManager::GetCurrentLocation() + '\\' + m_Data.Name, GetExecDir().append(m_Data.Name).string());
	}
	catch (const std::exception& exception)
	{
		m_ErrorMessage = exception.what();
		m_Success = false;
	}
}

void File::DisplayContextMenu()
{
	if (ImGui::Button("Download"))
	{
		Download();
		ImGui::CloseCurrentPopup();
	}
}
