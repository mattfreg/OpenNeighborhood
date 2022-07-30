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

    auto driveElements = UI::CreateDriveElements();

    if (driveElements.empty())
        return;

    ContentsChangeEvent event(driveElements);
    m_EventCallback(event);
}
