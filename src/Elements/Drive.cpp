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

    ImGui::SetNextWindowSize(ImVec2(380, 400));

    ImGui::Begin("Drive properties", &m_ShowPropertiesWindow, windowFlags);

    // Drive friendly name
    ImGui::TextUnformatted(m_Data.FriendlyName.c_str());

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();

    // Progress bars
    const char usedSpaceText[] = "Used space";
    const char freeSpaceText[] = "Free space";
    static ImVec2 usedSpaceTextSize = ImGui::CalcTextSize(usedSpaceText);
    static ImVec2 freeSpaceTextSize = ImGui::CalcTextSize(freeSpaceText);
    static float offset = std::max<float>(usedSpaceTextSize.x, freeSpaceTextSize.x) + ImGui::GetStyle().ItemSpacing.x * 2.0f;
    static float usedBytesFraction = static_cast<float>(m_Data.TotalUsedBytes) / static_cast<float>(m_Data.TotalBytes);
    static float freeBytesFraction = static_cast<float>(m_Data.TotalFreeBytes) / static_cast<float>(m_Data.TotalBytes);
    static std::string usedBytesCompactLabel = NumberFormatter::FileSize(m_Data.TotalUsedBytes);
    static std::string usedBytesCompleteLabel = NumberFormatter::Decimal(m_Data.TotalUsedBytes);
    static std::string freeBytesCompactLabel = NumberFormatter::FileSize(m_Data.TotalFreeBytes);
    static std::string freeBytesCompleteLabel = NumberFormatter::Decimal(m_Data.TotalFreeBytes);
    static std::string totalBytesCompactLabel = NumberFormatter::FileSize(m_Data.TotalBytes);
    static std::string totalBytesCompleteLabel = NumberFormatter::Decimal(m_Data.TotalBytes);

    // Used space progress bar
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
    ImGui::TextUnformatted(usedSpaceText);
    ImGui::SameLine(offset);
    ImGui::ProgressBar(usedBytesFraction, ImVec2(200.0f, 0.0f));
    ImGui::SameLine();
    ImGui::TextUnformatted(usedBytesCompactLabel.c_str());
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%s bytes", usedBytesCompleteLabel.c_str());
        ImGui::EndTooltip();
    }

    // Free space progress bar
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.18f, 0.63f, 0.10f, 1.0f));
    ImGui::TextUnformatted(freeSpaceText);
    ImGui::SameLine(offset);
    ImGui::ProgressBar(freeBytesFraction, ImVec2(200.0f, 0.0f));
    ImGui::SameLine();
    ImGui::TextUnformatted(freeBytesCompactLabel.c_str());
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%s bytes", freeBytesCompleteLabel.c_str());
        ImGui::EndTooltip();
    }

    ImGui::PopStyleColor(2);

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();

    // Total capacity
    ImGui::TextUnformatted("Total capacity:\t");
    ImGui::SameLine();
    ImGui::TextUnformatted(totalBytesCompactLabel.c_str());
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%s bytes", totalBytesCompleteLabel.c_str());
        ImGui::EndTooltip();
    }

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
