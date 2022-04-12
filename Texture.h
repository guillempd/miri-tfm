#pragma once

#include <glad/glad.h>

#include <string_view>

class Texture
{
public:
    Texture();
    void Load(std::string_view path);
    void SetUnit(unsigned int unit);
    GLuint m_id;
private:
    void LoadLdr(std::string_view path);
    void LoadHdr(std::string_view path);
};