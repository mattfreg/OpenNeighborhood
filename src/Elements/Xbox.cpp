#include "pch.h"
#include "Elements/Xbox.h"

#include "Helpers/ConsoleStore.h"
#include "Helpers/LocationMover.h"
#include "Events/AppEvent.h"
#include "Elements/Drive.h"
#include "Render/UI.h"

Xbox::Xbox(const std::string &label, const std::string &ipAddress)
    : m_IpAddress(ipAddress), Element(label, "xbox")
{
}

void Xbox::OnClick()
{
    XBDM::Console &console = ConsoleStore::GetConsole();

    // If the current console was created by clicking on the AddXboxButton, the connection is
    // already open so no need to recreate it
    if (!console.IsConnected())
    {
        UI::SetSuccess(ConsoleStore::CreateConsole(m_IpAddress));

        if (!UI::IsGood())
        {
            UI::SetErrorMessage("Couldn't find console");
            return;
        }
    }

    std::vector<XBDM::Drive> drives;

    bool success = ConsoleStore::Try([&]() { drives = console.GetDrives(); });

    if (!success)
        return;

    LocationMover::SetCurrentPosition(LocationMover::Position::DriveList);

    auto driveElements = std::vector<Ref<Element>>();
    driveElements.reserve(drives.size());

    for (auto &drive : drives)
        driveElements.emplace_back(CreateRef<Drive>(drive));

    ContentsChangeEvent event(driveElements);
    m_EventCallback(event);
}
