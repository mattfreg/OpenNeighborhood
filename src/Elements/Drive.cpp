#include "pch.h"
#include "Elements/Drive.h"

#include "Helpers/ConsoleStore.h"
#include "Helpers/LocationMover.h"
#include "Helpers/NumberFormatter.h"
#include "Events/AppEvent.h"
#include "Elements/File.h"
#include "Render/UI.h"

Drive::Drive(const XBDM::Drive &data)
    : Element(data.FriendlyName + " (" + data.Name + ")", "drive"), m_Data(data)
{
}

void Drive::OnClick()
{
    XBDM::Console &console = ConsoleStore::GetConsole();
    std::set<XBDM::File> files;

    bool success = ConsoleStore::Try([&]() { files = console.GetDirectoryContents(LocationMover::GetCurrentConsoleLocation() + m_Data.Name + '\\'); });

    if (!success)
        return;

    // Set the current location in the console to be the root of the current drive
    LocationMover::GoToDirectory(m_Data.Name);

    LocationMover::SetCurrentAppLocation(LocationMover::AppLocation::DriveContents);

    auto fileElements = std::vector<Ref<Element>>();
    fileElements.reserve(files.size());

    for (auto &file : files)
        fileElements.emplace_back(CreateRef<File>(file));

    ContentsChangeEvent event(fileElements);
    m_EventCallback(event);
}

void Drive::DisplayProperties()
{
    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowSize(ImVec2(380, 400));

    std::string driveNameWithoutColon = m_Data.Name.substr(0, m_Data.Name.size() - 1);
    std::string windowTitle = "Properties of " + driveNameWithoutColon;
    ImGui::Begin(windowTitle.c_str(), &m_ShowPropertiesWindow, windowFlags);

    // Drive name and friendly name
    const char driveNameText[] = "Drive:\t";
    const char driveTypeText[] = "Type:\t";
    ImVec2 driveNameTextSize = ImGui::CalcTextSize(driveNameText);
    ImVec2 driveTypeTextSize = ImGui::CalcTextSize(driveTypeText);
    float driveNamesOffset = std::max<float>(driveNameTextSize.x, driveTypeTextSize.x) + ImGui::GetStyle().ItemSpacing.x * 2.0f;

    ImGui::TextUnformatted(driveNameText);
    ImGui::SameLine(driveNamesOffset);
    ImGui::TextUnformatted(driveNameWithoutColon.c_str());
    ImGui::TextUnformatted(driveTypeText);
    ImGui::SameLine(driveNamesOffset);
    ImGui::TextUnformatted(m_Data.FriendlyName.c_str());

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();

    // Progress bars
    const char usedSpaceText[] = "Used space";
    const char freeSpaceText[] = "Free space";
    ImVec2 usedSpaceTextSize = ImGui::CalcTextSize(usedSpaceText);
    ImVec2 freeSpaceTextSize = ImGui::CalcTextSize(freeSpaceText);
    float progressBarsOffset = std::max<float>(usedSpaceTextSize.x, freeSpaceTextSize.x) + ImGui::GetStyle().ItemSpacing.x * 2.0f;

    // Used space progress bar
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
    ImGui::TextUnformatted(usedSpaceText);
    ImGui::SameLine(progressBarsOffset);
    ImGui::ProgressBar(static_cast<float>(m_Data.TotalUsedBytes) / static_cast<float>(m_Data.TotalBytes), ImVec2(200.0f, 0.0f));
    ImGui::SameLine();
    ImGui::TextUnformatted(NumberFormatter::FileSize(m_Data.TotalUsedBytes).c_str());
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%s bytes", NumberFormatter::Decimal(m_Data.TotalUsedBytes).c_str());
        ImGui::EndTooltip();
    }

    // Free space progress bar
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.18f, 0.63f, 0.10f, 1.0f));
    ImGui::TextUnformatted(freeSpaceText);
    ImGui::SameLine(progressBarsOffset);
    ImGui::ProgressBar(static_cast<float>(m_Data.TotalFreeBytes) / static_cast<float>(m_Data.TotalBytes), ImVec2(200.0f, 0.0f));
    ImGui::SameLine();
    ImGui::TextUnformatted(NumberFormatter::FileSize(m_Data.TotalFreeBytes).c_str());
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%s bytes", NumberFormatter::Decimal(m_Data.TotalFreeBytes).c_str());
        ImGui::EndTooltip();
    }

    ImGui::PopStyleColor(2);

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();

    // Total capacity
    ImGui::TextUnformatted("Total capacity:\t");
    ImGui::SameLine();
    ImGui::TextUnformatted(NumberFormatter::FileSize(m_Data.TotalBytes).c_str());
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%s bytes", NumberFormatter::Decimal(m_Data.TotalBytes).c_str());
        ImGui::EndTooltip();
    }

    ImGui::End();
}

void Drive::DisplayContextMenu()
{
    if (ImGui::BeginPopupContextItem())
    {
        ImGui::PushFont(UI::GetBoldFont());
        if (ImGui::Button("Open"))
        {
            OnClick();
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopFont();

        ImGui::Separator();

        if (ImGui::Button("Properties"))
        {
            m_ShowPropertiesWindow = true;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if (m_ShowPropertiesWindow)
        DisplayProperties();
}
