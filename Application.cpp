#include "Application.h"
#include "ImGuiNfd.h"
#include "Window.h"

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include <glad/glad.h>

#include <iostream>
#include <string>

Application::Application(int width, int height, Window* window)
    : m_camera()
    , m_previousCursorPosition()
    , m_physicalSky()
    , m_window(window)
    , m_exposure(-2.0f)
    , m_max_white(1e6f)
    , m_displayMode(DisplayMode::DAY)
    , m_blueTint(0.1f, 0.1f, 0.5f)
    , m_noiseScale(50.0f)
    , m_noiseStrength(0.005f)
    , m_noiseSpeed(0.05f)
    , m_mesopicRangeStart(0.0f)
    , m_mesopicRangeEnd(0.01f)
{
    std::cout << "Creating application" << std::endl;

    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    OnFramebufferSize(width, height);

    // FRAMEBUFFER STUFF
    glGenFramebuffers(1, &m_hdrFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFramebuffer);

    glGenTextures(1, &m_hdrTexture);
    glBindTexture(GL_TEXTURE_2D, m_hdrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, nullptr); // TODO: Correct width and height parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_hdrTexture, 0);

    glGenRenderbuffers(1, &m_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER , m_depthRenderbuffer);

    // POSTPROCESS STUFF
    ShaderStage vertexShader = ShaderStage();
    vertexShader.Create(ShaderType::VERTEX);
    vertexShader.Compile("D:/dev/miri-tfm/resources/shaders/postprocess.vert", "D:/dev/miri-tfm/resources/shaders");

    ShaderStage fragmentShader = ShaderStage();
    fragmentShader.Create(ShaderType::FRAGMENT);
    fragmentShader.Compile("D:/dev/miri-tfm/resources/shaders/postprocess.frag", "D:/dev/miri-tfm/resources/shaders");

    m_postprocessShader.Create();
    m_postprocessShader.AttachShader(vertexShader.m_id);
    m_postprocessShader.AttachShader(fragmentShader.m_id);
    m_postprocessShader.Build();

    // BUFFERS STUFF
    glGenVertexArrays(1, &m_fullScreenQuadVao);
    glBindVertexArray(m_fullScreenQuadVao);

    float vertices[] = {
        -1.0, -1.0, 0.0,
        +1.0, -1.0, 0.0,
        -1.0, +1.0, 0.0,
        +1.0, +1.0, 0.0
    };

    glGenBuffers(1, &m_fullScreenQuadVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullScreenQuadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
}

// TODO: Delete opengl resources
Application::~Application()
{
    // glDeleteFramebuffers(1, &m_hdrFramebuffer);
    // glDeleteTextures();
    std::cout << "Destroying Application" << std::endl;
}

void Application::OnCursorPos(double xpos, double ypos)
{
    glm::vec2 currentCursorPosition = glm::vec2(xpos, ypos);
    glm::vec2 cursorMovement = currentCursorPosition - m_previousCursorPosition;
    bool captured = false;
    if (!captured) captured = m_camera.OnCursorMovement(cursorMovement);

    m_previousCursorPosition = currentCursorPosition;
}

void Application::OnMouseButton(int button, int action, int mods)
{
    m_camera.OnMouseButton(button, action, mods);
}

void Application::OnFramebufferSize(int width, int height)
{
    m_resolution = glm::vec3(width, height, 0.0);
    glViewport(0, 0, width, height);
    m_camera.SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
    GLenum errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) std::cerr << "GL error after resize" << std::endl;
}

void Application::OnScroll(double xoffset, double yoffset)
{
    m_camera.OnScroll(static_cast<int>(yoffset));
}

void Application::OnUpdate()
{
    ImGui::ShowDemoWindow();
    ImGui::ShowMetricsWindow();

    m_camera.OnUpdate();
    m_physicalSky.Update();

    if (ImGui::Begin("Postprocess"))
    {
        ImGui::SliderFloat("Exposure", &m_exposure, -5.0f, 5.0f);
        ImGui::SliderFloat("White Point", &m_max_white, 0.0f, 1e6f, "%.3f", ImGuiSliderFlags_Logarithmic);
        ImGui::RadioButton("Day", reinterpret_cast<int*>(&m_displayMode), static_cast<int>(DisplayMode::DAY)); ImGui::SameLine();
        ImGui::RadioButton("Night", reinterpret_cast<int*>(&m_displayMode), static_cast<int>(DisplayMode::NIGHT)); ImGui::SameLine();
        ImGui::RadioButton("Photopic Luminance", reinterpret_cast<int*>(&m_displayMode), static_cast<int>(DisplayMode::PHOTOPIC_LUMINANCE)); ImGui::SameLine();
        ImGui::RadioButton("Scotopic Luminance", reinterpret_cast<int*>(&m_displayMode), static_cast<int>(DisplayMode::SCOTOPIC_LUMINANCE));
        if (m_displayMode == DisplayMode::NIGHT)
        {
            ImGui::ColorEdit3("Tint Color", glm::value_ptr(m_blueTint), ImGuiColorEditFlags_Float);
            ImGui::SliderFloat("Noise Scale", &m_noiseScale, 0.0f, 200.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat("Noise Strength", &m_noiseStrength, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat("Noise Speed", &m_noiseSpeed, 0.001f, 0.1f);
            ImGui::SliderFloat("Mesopic Range Start", &m_mesopicRangeStart, 0.0f, 1.0f);
            ImGui::SliderFloat("Mesopic Range End", &m_mesopicRangeEnd, 0.0f, 1.0f);
            if (m_mesopicRangeStart > m_mesopicRangeEnd) m_mesopicRangeStart = m_mesopicRangeEnd;
        }
    }
    ImGui::End();

    /*if (ImGui::Begin("Model"))
    {
        nfdfilteritem_t meshFilter = { "3D mesh", "gltf" };
        if (ImGui::Button("Open other model..."))
        {
            std::string path = ImGuiNfd::Load(&meshFilter, 1);
            if (path != "") m_mesh = std::make_unique<Mesh>(path);
        }
    }
    ImGui::End();*/
}

void Application::OnRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_physicalSky.Render(m_camera);

    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_postprocessShader.Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_hdrTexture);
    m_postprocessShader.SetInt("hdrTexture", 0);
    m_postprocessShader.SetFloat("Exposure", glm::pow(10.0f, m_exposure));
    m_postprocessShader.SetFloat("max_white", m_max_white);

    m_postprocessShader.SetFloat("AspectRatio", m_resolution.x / m_resolution.y);
    m_postprocessShader.SetFloat("Time", m_window->GetTime());

    m_postprocessShader.SetVec3("BlueTint", m_blueTint);
    m_postprocessShader.SetFloat("NoiseScale", m_noiseScale);
    m_postprocessShader.SetFloat("NoiseStrength", m_noiseStrength);
    m_postprocessShader.SetFloat("NoiseSpeed", m_noiseSpeed);
    m_postprocessShader.SetVec3("MesopicRange", glm::vec3(m_mesopicRangeStart, m_mesopicRangeEnd, 0.0f));

    m_postprocessShader.SetInt("Mode", static_cast<int>(m_displayMode));

    glBindVertexArray(m_fullScreenQuadVao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    GLenum errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) std::cerr << "[OpenGL] E: Something failed while rendering Physical Sky." << std::endl;
}