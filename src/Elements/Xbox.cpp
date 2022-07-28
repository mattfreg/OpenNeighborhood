#include "pch.h"
#include "Elements/Xbox.h"

#include "Xbox/XboxManager.h"
#include "Events/AppEvent.h"
#include "Elements/Drive.h"
#include "Render/UI.h"

Xbox::Xbox(const std::string &label, const std::string &ipAddress)
    : m_IpAddress(ipAddress), Element(label, "xbox")
{
}

void Xbox::OnClick()
{
    XBDM::Console &xbox = XboxManager::GetConsole();

    // If the current xbox was created by clicking on the AddXboxButton, the connection is
    // already open so no need to recreate it
    if (!xbox.IsConnected())
    {
        UI::SetSuccess(XboxManager::CreateConsole(m_IpAddress));

        if (!UI::IsGood())
        {
            UI::SetErrorMessage("Couldn't find console");
            return;
        }
    }

    std::vector<XBDM::Drive> drives;

    bool success = XboxManager::Try([&]() { drives = xbox.GetDrives(); });

    if (!success)
        return;

    XboxManager::SetCurrentPosition(XboxManager::Position::DriveList);

    auto driveElements = std::vector<Ref<Element>>();

    for (auto &drive : drives)
        driveElements.emplace_back(CreateRef<Drive>(drive));

    ContentsChangeEvent event(driveElements);
    m_EventCallback(event);
}
