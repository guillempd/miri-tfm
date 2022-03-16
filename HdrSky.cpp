#include "HdrSky.h"

#include <nfd.hpp>

#include <imgui.h>

#include <iostream>

void HdrSky::Load()
{
    const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    out vec2 texCoord;
    void main()
    {
        gl_Position = vec4(aPos.x, aPos.y, 1.0, 1.0);
        texCoord = 0.5 * (aPos.xy + vec2(1.0));
    }
)";

    const char* fragmentShaderSource = R"(
    #version 330 core
    in vec2 texCoord;
    out vec4 FragColor;
    uniform sampler2D hdrImage;
    uniform float verticalFov;
    uniform vec3 cameraRight;
    uniform vec3 cameraUp;
    uniform vec3 cameraForward;
    uniform float aspectRatio;
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
        // NOTE: Be careful with all z coordinates (might be negated)
        const vec3 cameraPosition = vec3(0.0);
        float near = 0.1;

        mat3 cameraRotation = mat3(cameraRight, cameraUp, cameraForward);
        vec2 ndc = 2 * texCoord - vec2(1.0);
        float top = near * tan(verticalFov/2);
        float right = top * aspectRatio;
        vec2 viewOffset = ndc * vec2(right, top);
        vec3 ray = cameraPosition + cameraRotation * vec3(viewOffset, near); // FIXME: cameraPosition should not be used here
        vec3 direction = normalize(ray);
        // Sample with this direction the hdrImage

        FragColor = texture(hdrImage, SampleSphericalMap(direction));
        FragColor = FragColor / (1.0 + FragColor);
        FragColor.a = 1.0;
        // if (aspectRatio > 1.0) FragColor = vec4(0.0);
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
    m_program.SetFloat("verticalFov", camera.GetVerticalFov());
    m_program.SetVec3("cameraRight", camera.GetRight());
    m_program.SetVec3("cameraUp", camera.GetUp());
    m_program.SetVec3("cameraForward", camera.GetForward());
    m_program.SetFloat("aspectRatio", camera.GetAspectRatio());

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
