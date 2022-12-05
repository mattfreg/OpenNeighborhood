#include "pch.h"
#include "Elements/Xbox.h"

#include "Helpers/ConsoleStore.h"
#include "Helpers/LocationMover.h"
#include "Events/AppEvent.h"
#include "Elements/Drive.h"
#include "Render/UI.h"

Xbox::Xbox(const std::string &label, const std::string &ipAddress)
    : Element(label, "xbox"), m_IpAddress(ipAddress)
{
}

void Xbox::OnClick()
{
    XBDM::Console &console = ConsoleStore::GetConsole();
    if (!console.IsConnected())
        ConnectToConsole();

    auto driveElements = UI::CreateDriveElements();

    if (driveElements.empty())
        return;

    ContentsChangeEvent event(driveElements);
    m_EventCallback(event);
}

void Xbox::SynchronizeTime()
{
    XBDM::Console &console = ConsoleStore::GetConsole();
    if (!console.IsConnected())
        ConnectToConsole();

    bool success = ConsoleStore::Try([&]() { ConsoleStore::GetConsole().SynchronizeTime(); });

    if (!success)
        return;

    UI::SetSuccessMessage("Successfully synchronized the console time with PC time.");
    UI::SetDisplaySuccessModal(true);
}

bool Xbox::FetchConsoleInfo()
{
    XBDM::Console &console = ConsoleStore::GetConsole();
    if (!console.IsConnected())
        ConnectToConsole();

    bool success = ConsoleStore::Try([&]() { m_ActiveTitle = console.GetActiveTitle(); });

    if (!success)
        return false;

    return ConsoleStore::Try([&]() { m_ConsoleType = console.GetType(); });
}

void Xbox::ConnectToConsole()
{
    UI::SetSuccess(ConsoleStore::CreateConsole(m_IpAddress));

    if (!UI::IsGood())
        UI::SetErrorMessage("Couldn't find console");
}

void Xbox::DisplayProperties()
{
    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowSize(ImVec2(380, 400));

    // The console name is cached by XBDM so it is completely fine to request the console name each frame,
    // a packet will be sent over the network only the first time
    const std::string &consoleName = ConsoleStore::GetConsole().GetName();
    std::string windowTitle = "Properties of " + consoleName;
    ImGui::Begin(windowTitle.c_str(), &m_ShowPropertiesWindow, windowFlags);

    // Console name and type
    const char consoleNameText[] = "Name:\t";
    const char consoleTypeText[] = "Type:\t";
    ImVec2 consoleNameTextSize = ImGui::CalcTextSize(consoleNameText);
    ImVec2 consoleTypeTextSize = ImGui::CalcTextSize(consoleTypeText);
    float consoleNameAndTypeOffset = std::max<float>(consoleNameTextSize.x, consoleTypeTextSize.x) + ImGui::GetStyle().ItemSpacing.x * 2.0f;

    ImGui::TextUnformatted(consoleNameText);
    ImGui::SameLine(consoleNameAndTypeOffset);
    ImGui::TextUnformatted(consoleName.c_str());
    ImGui::TextUnformatted(consoleTypeText);
    ImGui::SameLine(consoleNameAndTypeOffset);
    ImGui::TextUnformatted(m_ConsoleType.c_str());

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();

    // IP address
    ImGui::Text("IP address:\t%s", m_IpAddress.c_str());

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();

    // Active title
    ImGuiWindowFlags childWindowFlags = ImGuiWindowFlags_NoScrollbar;
    float childWindowHeight = ImGui::GetTextLineHeight() + ImGui::GetStyle().WindowPadding.y * 2.0f;
    ImGui::TextUnformatted("Active title:\t");
    ImGui::SameLine();

    // Invisible window around the active title path to make the path horizontally scrollable
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
    ImGui::BeginChild("Active title", ImVec2(0.0f, childWindowHeight), false, childWindowFlags);
    ImGui::TextUnformatted(m_ActiveTitle.c_str());
    ImGui::EndChild();
    ImGui::PopStyleVar();

    ImGui::End();
}

void Xbox::DisplayContextMenu()
{
    if (ImGui::BeginPopupContextItem())
    {
        ImVec2 buttonSize = { ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2.0f, 0.0f };
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.0f));

        ImGui::PushFont(UI::GetBoldFont());
        if (ImGui::Button("Open", buttonSize))
        {
            OnClick();
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopFont();

        ImGui::Separator();

        if (ImGui::Button("Synchronize Time", buttonSize))
        {
            SynchronizeTime();
            ImGui::CloseCurrentPopup();
        }

        ImGui::Separator();

        if (ImGui::Button("Properties", buttonSize))
        {
            // Show the properties window only if fetching the active title succeeds
            m_ShowPropertiesWindow = FetchConsoleInfo();
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopStyleVar();
        ImGui::EndPopup();
    }

    if (m_ShowPropertiesWindow)
        DisplayProperties();
}
