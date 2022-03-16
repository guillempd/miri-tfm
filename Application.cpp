#include "Application.h"

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include <glad/glad.h>

#include <iostream>

Application::Application()
    : m_camera()
    , m_previousCursorPosition()
    , m_hdrSky()
    , m_physicalSky(1, 1)
    , m_skyType(SkyType::HDR)
{
    std::cout << "Creating Application" << std::endl;
}

Application::~Application()
{
    std::cout << "Destroying Application" << std::endl;
}

void Application::Initialize(int width, int height, Window* window)
{
    m_window = window;
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    m_hdrSky.Load();
    m_physicalSky.Initialize(m_window);
    m_mesh.Load();
    OnFramebufferSize(width, height);
}

void Application::OnCursorMovement(double xpos, double ypos)
{
    glm::vec2 currentCursorPosition = glm::vec2(xpos, ypos);
    glm::vec2 cursorMovement = currentCursorPosition - m_previousCursorPosition;

    m_camera.OnMouseMovement(cursorMovement);
    m_physicalSky.OnMouseMovement(cursorMovement);

    m_previousCursorPosition = currentCursorPosition;
}

void Application::OnMouseClick(int button, int action, int mods)
{
    m_camera.OnMouseClick(button, action, mods);
    m_physicalSky.OnMouseClick(button, action, mods);
}

void Application::OnFramebufferSize(int width, int height)
{
    glViewport(0, 0, width, height);
    m_camera.SetAspectRatio(width, height);
    m_physicalSky.HandleReshapeEvent(width, height);
    GLenum errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) std::cerr << "GL error after resize" << std::endl;
}

void Application::OnScroll(double xoffset, double yoffset)
{
    m_physicalSky.HandleMouseWheelEvent(static_cast<int>(yoffset));
}

void Application::OnRender()
{
    ImGui::ShowDemoWindow();
    m_camera.OnRender();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (ImGui::Begin("Sky Selection"))
    {
        ImGui::RadioButton("HDR Sky", reinterpret_cast<int*>(&m_skyType), static_cast<int>(SkyType::HDR));
        ImGui::RadioButton("Physical Sky", reinterpret_cast<int*>(&m_skyType), static_cast<int>(SkyType::PHYSICAL));
    }
    ImGui::End();

    m_mesh.Render(m_camera);

    switch (m_skyType)
    {
    case SkyType::HDR:
    {
        m_hdrSky.Render(m_camera);
        GLenum errorCode = glGetError();
        if (errorCode != GL_NO_ERROR) std::cerr << "GL error after rendering HDR Sky" << std::endl;
    } break;
    case SkyType::PHYSICAL:
    {
        m_physicalSky.HandleRedisplayEvent();
        GLenum errorCode = glGetError();
        if (errorCode != GL_NO_ERROR) std::cerr << "GL error after rendering physical sky" << std::endl;
    } break;
    default:
    {
        std::cout << "E: Unknown sky type selected." << std::endl;
    }
    }
}