#pragma once

#include "ShaderProgram.h"
#include "Texture.h"
#include "Camera.h"

#include <glad/glad.h>

class HdrSky
{
public:
    HdrSky() = default;
    ~HdrSky() = default; // TODO: Delete buffers appropriately
    void Init();
    void OnUpdate();
    void OnRender(const Camera& camera);
private:
    void LoadFullScreenQuad();
private:
    ShaderProgram m_program;
    Texture m_texture;
    GLuint m_vao;
    GLuint m_vbo;
};