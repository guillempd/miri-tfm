#include "HdrSky.h"

#include <nfd.hpp>

#include <imgui.h>

#include <iostream>

void HdrSky::Load()
{
    const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    uniform mat4 view_from_clip;
    uniform mat4 world_from_view;
    out vec3 view_ray;
    void main()
    {
        view_ray = (world_from_view * view_from_clip * vec4(aPos.x, aPos.y, 1.0, 0.0)).xyz;
        gl_Position = vec4(aPos.x, aPos.y, 1.0, 1.0);
    }
)";

    const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 view_ray;
    out vec4 FragColor;
    uniform sampler2D hdrImage;
    // TODO: Check this code extracted from learnopengl.com
    const vec2 invAtan = vec2(0.1591, 0.3183);
    vec2 SampleSphericalMap(vec3 v)
    {
        vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
        uv *= invAtan;
        uv += 0.5;
        return uv;
    }
    void main()
    {
        vec3 direction = normalize(view_ray);
        // Sample with this direction the hdrImage
        FragColor = texture(hdrImage, SampleSphericalMap(direction));
        FragColor = FragColor / (1.0 + FragColor); // Tonemapping
        FragColor.a = 1.0;
    }
)";

    m_program.SetVertexShaderSource(vertexShaderSource);
    m_program.SetFragmentShaderSource(fragmentShaderSource);
    m_program.Build();

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

void HdrSky::Render(const Camera& camera)
{
    RenderUi();

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

void HdrSky::RenderUi()
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
