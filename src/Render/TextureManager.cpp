#include "pch.h"
#include "Render/TextureManager.h"

#include "Core/Assert.h"

std::unordered_map<std::string, Ref<Texture>> TextureManager::s_Textures;
uint32_t TextureManager::s_TextureCount = 0;

void TextureManager::AddTexture(const std::string &textureName, const std::filesystem::path &filePath)
{
    ASSERT(s_Textures.find(textureName) == s_Textures.end(), "Texture with name: %s already exists!", textureName.c_str());

    s_Textures[textureName] = CreateRef<Texture>(filePath);
    s_Textures[textureName]->Bind(s_TextureCount);
    s_TextureCount++;
}

Ref<Texture> TextureManager::GetTexture(const std::string &textureName)
{
    auto texture = s_Textures.find(textureName);

    ASSERT(texture != s_Textures.end(), "Couldn't find texture!");

    return texture->second;
}

bool TextureManager::TextureExists(const std::string &textureName)
{
    return s_Textures.find(textureName) != s_Textures.end();
}
