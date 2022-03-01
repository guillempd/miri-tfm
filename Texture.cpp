#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

Texture::Texture()
    : m_id(GL_NONE)
{
}

void Texture::Load()
{
    int x, y, n;
    stbi_set_flip_vertically_on_load(true);
    float* data = stbi_loadf("D:/Descargas/drackenstein_quarry_4k.hdr", &x, &y, &n, 0);
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
