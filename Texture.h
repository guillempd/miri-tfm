#pragma once

#include <glad/glad.h>

#include <string_view>

class Texture
{
public:
    Texture();
    void Load(std::string_view imagePath);
    void SetUnit(unsigned int unit);
    GLuint m_id;
private:
};