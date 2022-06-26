#pragma once

class Texture
{
public:
    Texture(const std::string &filePath);
    ~Texture();

    void Bind(uint32_t slot = 0) const;

    inline uint32_t GetTextureID() const { return m_RendererID; }

    inline float GetWidth() const { return m_Width; }

    inline float GetHeight() const { return m_Height; }

private:
    uint32_t m_RendererID;
    float m_Width;
    float m_Height;
};
