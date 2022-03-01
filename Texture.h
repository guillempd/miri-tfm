#pragma once

#include <glad/glad.h>

class Texture
{
public:
    Texture();
    void Load();
    void SetUnit(unsigned int unit);
    GLuint m_id;
private:
};