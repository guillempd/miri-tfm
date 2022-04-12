#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

// TODO: Manage texture resource (delete it at the end)
Texture::Texture()
    : m_id(GL_NONE)
{
}

// TODO: Load non hdr textures

// This is for loading hdr textures (also valid for ldr textures)
void Texture::Load(std::string_view path)
{
    if (stbi_is_hdr(path.data())) LoadHdr(path);
    else LoadLdr(path);
}

void Texture::LoadLdr(std::string_view path)
{
    int x, y, n;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.data(), &x, &y, &n, 3); // request 3 components (?)
    glGenTextures(1, &m_id);

    if (data)
    {
        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "[stbi] E: Could not load image." << std::endl;
    }
}

void Texture::LoadHdr(std::string_view path)
{
    int x, y, n;
    stbi_set_flip_vertically_on_load(true);
    float* data = stbi_loadf(path.data(), &x, &y, &n, 0);
    glGenTextures(1, &m_id);

    if (data)
    {
        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, x, y, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else // TODO: Set default texture, for example pink
    {
        std::cerr << "[stbi] E: Could not load image." << std::endl;
    }
}

void Texture::SetUnit(unsigned int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_id);
}
