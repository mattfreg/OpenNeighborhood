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

	std::filesystem::path localPath = GetExecDir().append(m_Data.Name);
	std::string& localPathString = localPath.string();

	/**
	 * It's important to capture localPath by copy because it will be destroyed
	 * by the time doDownload is called if it's called as the confirm callback,
	 * capturing it by reference would create a crash.
	 */
	auto download = [this, localPath]()
	{
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
	};

	m_ConfirmCallback = download;

	if (std::filesystem::exists(localPath))
	{
		m_ConfirmMessage = "A file named \"" + m_Data.Name + "\" already exists, do you want to overwrite it?";
		m_Confirm = true;
		return;
	}

	download();
}

void File::DisplayContextMenu()
{
	if (ImGui::Button("Download"))
	{
		Download();
		ImGui::CloseCurrentPopup();
	}
}
