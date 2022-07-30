#include "pch.h"
#include "Elements/Element.h"

#include "Render/TextureMap.h"
#include "OpenNeighborhood.h"
#include "Render/UI.h"

Element::Element(const std::string &label, const std::string &textureName)
    : m_Label(label), m_TextureName(textureName)
{
    m_EventCallback = BIND_EVENT_FN(Element::OnEvent);

    if (textureName.empty())
        return;

    if (!TextureMap::TextureExists(textureName))
        TextureMap::AddTexture(textureName, GetExecDir().append("assets").append("icons").append(textureName + ".png"));

    auto texture = TextureMap::GetTexture(textureName);
    m_Width = texture->GetWidth() * 3;
    m_Height = texture->GetHeight();
}

void Element::OnRender()
{
    auto texture = TextureMap::GetTexture(m_TextureName);

    if (ImGui::ImageButtonWithText(reinterpret_cast<void *>(static_cast<intptr_t>(texture->GetTextureID())), ImVec2(texture->GetWidth(), texture->GetHeight()), ImVec2(m_Width, m_Height), m_Label.c_str(), ImVec2(m_Padding, m_Padding)))
        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            OnClick();

    DisplayContextMenu();
}

void Element::OnEvent(Event &event)
{
    OpenNeighborhood &currentLayer = OpenNeighborhood::Get();
    currentLayer.OnEvent(event);
}
