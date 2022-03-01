#include "Application.h"

#include <imgui.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

Application::Application()
    : m_clearColor{1.0f, 1.0f, 1.0f}
    , m_verticalFov(3.1415f / 4.0f)
    , m_vao(GL_NONE)
    , m_vbo(GL_NONE)
    , m_program(GL_NONE)
    , m_texture(GL_NONE)
{
    std::cout << "Creating Application" << std::endl;
}

Application::~Application()
{
    std::cout << "Destroying Application" << std::endl;
}

void Application::Initialize()
{
    LoadFullScreenQuad();
    LoadShaders();
    LoadHdrImage();
}

void Application::OnCursorMovement(double xpos, double ypos)
{
    if (ImGui::GetIO().WantCaptureMouse) return;

    std::cout << "Cursor moved to position (" << xpos << ", " << ypos << ")" << std::endl;
}


void Application::LoadFullScreenQuad()
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

void Application::LoadShaders()
{
    const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    out vec2 texCoord;
    void main()
    {
        gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
        texCoord = 0.5 * (aPos.xy + vec2(1.0));
    }
)";

    const char* fragmentShaderSource = R"(
    #version 330 core
    in vec2 texCoord;
    out vec4 FragColor;
    uniform sampler2D hdrImage;
    uniform float verticalFov;
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
        // These would be uniforms
        const vec3 cameraRight = vec3(1.0, 0.0, 0.0);
        const vec3 cameraUp = vec3(0.0, 1.0, 0.0);
        const vec3 cameraForward = vec3(0.0, 0.0, 1.0);
        const vec3 cameraPosition = vec3(0.0);
        // float verticalFov = 3.1415/4;
        float ar = float(4)/float(3);
        float near = 0.1;

        const mat3 cameraRotation = mat3(cameraRight, cameraUp, cameraForward);
        vec2 ndc = 2 * texCoord - vec2(1.0);
        float top = near * tan(verticalFov/2);
        float right = top * ar;
        vec2 viewOffset = ndc * vec2(top, right);
        vec3 ray = cameraPosition + cameraRotation * vec3(viewOffset, -near); // FIXME: cameraPosition should not be used here
        vec3 direction = normalize(ray);
        // Sample with this direction the hdrImage

        FragColor = texture(hdrImage, SampleSphericalMap(direction));
    }
)";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    m_program = glCreateProgram();
    glAttachShader(m_program, vertexShader);
    glAttachShader(m_program, fragmentShader);
    glLinkProgram(m_program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint success;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[256];
        glGetProgramInfoLog(m_program, 512, NULL, infoLog);
        std::cerr << infoLog << std::endl;
    }
}

void Application::LoadHdrImage()
{
    int x, y, n;
    stbi_set_flip_vertically_on_load(true);
    float* data = stbi_loadf("D:/Descargas/drackenstein_quarry_4k.hdr", &x, &y, &n, 0);

    if (data)
    {
        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, x, y, 0, GL_RGB, GL_FLOAT, data);

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

void Application::OnRender()
{
    ImGui::ShowDemoWindow();

    if (ImGui::Begin("Color picker"))
    {
        ImGui::ColorEdit3("Clear Color", m_clearColor);
        ImGui::SliderFloat("Vertical Fov", &m_verticalFov, 3.1415f / 8.0f, 3.1415f / 2.0f);
    }
    ImGui::End();

    glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glUseProgram(m_program);
    glUniform1i(glGetUniformLocation(m_program, "hdrImage"), 0);
    glUniform1f(glGetUniformLocation(m_program, "verticalFov"), m_verticalFov);
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}