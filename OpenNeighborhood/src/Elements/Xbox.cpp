#include "pch.h"
#include "Elements/Xbox.h"

#include "Xbox/XboxManager.h"
#include "Events/AppEvent.h"
#include "Elements/Drive.h"
#include "Render/UI.h"


Xbox::Xbox(const std::string &label, const std::string &ipAddress)
    : m_IpAddress(ipAddress), Element(label, "xbox") {}

void Xbox::OnClick()
{
    std::string consoleName;
    UI::SetSuccess(XboxManager::CreateConsole(m_IpAddress, consoleName, true));

    if (!UI::IsGood())
    {
        UI::SetErrorMessage("Couldn't find console");
        return;
    }

    XBDM::Console &xbox = XboxManager::GetConsole();
    std::vector<XBDM::Drive> drives;

    bool success = XboxManager::Try([&]() { drives = xbox.GetDrives(); });

    if (!success)
        return;

    auto driveElements = CreateRef<std::vector<Ref<Element>>>();

    for (auto &drive : drives)
        driveElements->emplace_back(CreateRef<Drive>(drive));

    ContentsChangeEvent event(driveElements);
    m_EventCallback(event);
}
