#include "pch.h"
#include "Elements/File.h"

#include <nfd.hpp>

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

	/**
	 * Depending on the system, std::filesystem::path::native can return either
	 * std::wstring or std::string. Since we don't know, we are just using auto.
	 */
	auto extension = std::filesystem::path(m_Data.Name).extension().native().substr(1);
	auto filterName = extension;
	std::transform(filterName.begin(), filterName.end(), filterName.begin(), [](auto c) { return std::toupper(c); });

	NFD::UniquePathN outPath;
	nfdnfilteritem_t filterItem[] = { { filterName.c_str(), extension.c_str() } };
	nfdresult_t result = NFD::SaveDialog(outPath, filterItem, 1);

	if (result == NFD_ERROR)
	{
		m_ErrorMessage = NFD::GetError();
		m_Success = false;
		return;
	}
	
	if (result == NFD_CANCEL)
		return;

	std::filesystem::path localPath = outPath.get();

	XBDM::Console& xbox = XboxManager::GetConsole();

	try
	{
		xbox.ReceiveFile(XboxManager::GetCurrentLocation() + '\\' + m_Data.Name, localPath.string());
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
