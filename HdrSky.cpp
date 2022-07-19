#include "HdrSky.h"

#include <nfd.hpp>

#include <imgui.h>

#include <iostream>

void HdrSky::Init()
{
    ShaderStage vertexShaderSource = ShaderStage("D:/dev/miri-tfm/resources/shaders/hdrSky.vert");
    ShaderStage fragmentShaderSource = ShaderStage("D:/dev/miri-tfm/resources/shaders/hdrSky.frag");
    m_program.Build(vertexShaderSource, fragmentShaderSource);

    m_texture.Load("D:/Descargas/drackenstein_quarry_4k.hdr");

    LoadFullScreenQuad();
}

void HdrSky::LoadFullScreenQuad()
{
    float quadVertices[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f,

        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f
    };

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW); // TODO: Is this with &quadVertices (?)

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0); // TODO: Be careful with the offset
}

void HdrSky::OnUpdate()
{
    if (ImGui::Begin("HDR Sky Settings"))
    {
        ImGui::Text("HDR Image");
        ImGui::Text("whatever");
        ImGui::SameLine();
        if (ImGui::Button("Open..."))
        {
            NFD::UniquePath path;
            NFD::OpenDialog(path);
            if (path)
            {
                std::cout << "Opened file: " << path << std::endl;
                m_texture.Load(path.get());
            }
        }
    }
    ImGui::End();
}

void HdrSky::OnRender(const Camera& camera)
{
    m_texture.SetUnit(0);
    m_program.Use();
    m_program.SetInt("hdrImage", 0);
    m_program.SetMat4("world_from_view", camera.GetWorldFromViewMatrix());
    m_program.SetMat4("view_from_clip", camera.GetViewFromClipMatrix());

    GLint previousDepthFunc;
    glGetIntegerv(GL_DEPTH_FUNC, &previousDepthFunc);
    glDepthFunc(GL_LEQUAL);
    {
        glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glDepthFunc(previousDepthFunc);
}
