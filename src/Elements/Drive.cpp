#include "pch.h"
#include "Elements/Drive.h"

#include "Helpers/ConsoleStore.h"
#include "Helpers/LocationMover.h"
#include "Helpers/NumberFormatter.h"
#include "Events/AppEvent.h"
#include "Elements/File.h"
#include "Render/UI.h"

Drive::Drive(const XBDM::Drive &data)
    : Element(data.FriendlyName, "drive"), m_Data(data)
{
}

void Drive::OnClick()
{
    // Convert the drive name to lower case
    std::for_each(m_Data.Name.begin(), m_Data.Name.end(), [](unsigned char c) { c = static_cast<unsigned char>(std::tolower(c)); });

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

    ImGui::SetNextWindowSize(ImVec2(380, 450));

    ImGui::Begin("Drive properties", &m_ShowPropertiesWindow, windowFlags);

    ImGui::TextUnformatted(m_Data.FriendlyName.c_str());

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();

    const char usedSpaceText[] = "Used space";
    const char freeSpaceText[] = "Free space";
    ImVec2 usedSpaceTextSize = ImGui::CalcTextSize(usedSpaceText);
    ImVec2 freeSpaceTextSize = ImGui::CalcTextSize(freeSpaceText);
    float offset = std::max<float>(usedSpaceTextSize.x, freeSpaceTextSize.x) + ImGui::GetStyle().ItemSpacing.x * 2.0f;

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
    ImGui::TextUnformatted(usedSpaceText);
    ImGui::SameLine(offset);
    ImGui::ProgressBar(static_cast<float>(m_Data.TotalUsedBytes) / static_cast<float>(m_Data.TotalBytes), ImVec2(200.0f, 0.0f));
    ImGui::SameLine();
    ImGui::TextUnformatted(NumberFormatter::FileSize(m_Data.TotalUsedBytes).c_str());
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%s bytes", NumberFormatter::Decimal(m_Data.TotalUsedBytes).c_str());
        ImGui::EndTooltip();
    }

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.18f, 0.63f, 0.10f, 1.0f));
    ImGui::TextUnformatted(freeSpaceText);
    ImGui::SameLine(offset);
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

    ImGui::End();
}

void Drive::DisplayContextMenu()
{
    if (ImGui::BeginPopupContextItem())
    {
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
