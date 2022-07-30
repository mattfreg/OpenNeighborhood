#include "pch.h"
#include "Panels/PathPanel.h"

#include "Helpers/ConsoleStore.h"
#include "Helpers/LocationMover.h"
#include "Render/UI.h"
#include "Render/TextureMap.h"

PathPanel::PathPanel()
{
    std::string textureName = "leftArrow";

    if (!TextureMap::TextureExists(textureName))
        TextureMap::AddTexture(textureName, GetExecDir().append("assets").append("icons").append(textureName + ".png"));

    m_Width = m_WindowWidth - m_Margin * 2.0f;
    m_Height = m_Margin * 2.4f;

    m_PathNodes.emplace_back("OpenNeighborhood", std::string::npos, this);
}

void PathPanel::OnRender()
{
    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_HorizontalScrollbar;

    ImGui::SetNextWindowPos(ImVec2(m_Margin, m_Margin));
    ImGui::SetNextWindowSize(ImVec2(m_Width, m_Height));

    ImGui::Begin("Path Window", nullptr, windowFlags);
    ImGui::PushFont(UI::GetRegularBigFont());

    m_GoToParentButton.OnRender();

    ImGui::SameLine();
    ImGui::TextUnformatted("");

    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 &borderColor = style.Colors[ImGuiCol_Border];
    auto texture = TextureMap::GetTexture(m_GoToParentButton.GetTextureName());
    float lineX = m_Margin + static_cast<float>(texture->GetWidth()) + style.ItemSpacing.x * 3.0f;
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    drawList->AddLine(
        ImVec2(lineX, m_Margin),
        ImVec2(lineX, m_Margin + m_Height),
        IM_COL32(borderColor.x * 255.0f, borderColor.y * 255.0f, borderColor.z * 255.0f, borderColor.w * 255.0f)
    );

    ImGui::SameLine();

    for (size_t i = 0; i < m_PathNodes.size(); i++)
    {
        ImGui::PushID(static_cast<int>(i));

        m_PathNodes[i].OnRender();

        // Render all PathNodes on the same line and add a '>' between them
        if (i + 1 < m_PathNodes.size())
        {
            ImGui::SameLine();
            ImGui::TextUnformatted(">");
            ImGui::SameLine();
        }

        ImGui::PopID();
    }

    ImGui::PopFont();
    ImGui::End();

    if (!m_ContentsChangeEventQueue.empty())
    {
        UpdateDirectories();
        m_ContentsChangeEventQueue.pop();
    }
}

void PathPanel::OnEvent(Event &event)
{
    Panel::OnEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<ContentsChangeEvent>(BIND_EVENT_FN(PathPanel::OnCurrentXboxLocationChange));
}

bool PathPanel::OnCurrentXboxLocationChange(ContentsChangeEvent &event)
{
    m_ContentsChangeEventQueue.push(event);

    return true;
}

void PathPanel::UpdateDirectories()
{
    m_PathNodes.erase(m_PathNodes.begin() + 1, m_PathNodes.end());

    // If the current position is at the drive list or further (inside of a drive),
    // add a PathNode with the console name
    if (LocationMover::GetCurrentPosition() >= LocationMover::Position::DriveList)
        m_PathNodes.emplace_back(ConsoleStore::GetConsole().GetName(), std::string::npos, this);

    if (LocationMover::GetCurrentPosition() < LocationMover::Position::DriveContents)
        return;

    std::string locationCopy = LocationMover::GetCurrentLocation() + '\\';
    size_t pos = 0;

    // Split the current location with '\' and create a PathNode for each directory
    while ((pos = locationCopy.find('\\')) != std::string::npos)
    {
        std::string directory = locationCopy.substr(0, pos);

        if (!directory.empty())
            m_PathNodes.emplace_back(directory, m_PathNodes.size(), this);

        locationCopy.erase(0, pos + 1);
    }
}
