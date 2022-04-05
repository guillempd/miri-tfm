#pragma once

#include "ShaderProgram.h"
#include "Texture.h"
#include "Camera.h"

#include <glad/glad.h>

class HdrSky
{
public:
    HdrSky() = default;
    void Load();
    void Render(const Camera& camera);
    void RenderUi();
private:
    void LoadFullScreenQuad();
private:
    ShaderProgram m_program;
    Texture m_texture;
    GLuint m_vao;
    GLuint m_vbo;
};