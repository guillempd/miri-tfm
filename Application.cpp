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
    , m_skyType(SkyType::PHYSICAL)
{
    std::cout << "Creating Application" << std::endl;
}

Application::~Application()
{
    std::cout << "Destroying Application" << std::endl;
}

void Application::Initialize(int width, int height)
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_hdrSky.Load();
    m_physicalSky.Initialize();
    OnFramebufferSize(width, height);
}

void Application::OnCursorMovement(double xpos, double ypos)
{
    glm::vec2 currentCursorPosition = glm::vec2(xpos, ypos);
    glm::vec2 cursorMovement = currentCursorPosition - m_previousCursorPosition;
    m_camera.OnMouseMovement(cursorMovement);

    m_previousCursorPosition = currentCursorPosition;
}

void Application::OnMouseClick(int button, int action, int mods)
{
    m_camera.OnMouseClick(button, action, mods);
}

void Application::OnFramebufferSize(int width, int height)
{
    glViewport(0, 0, width, height);
    m_camera.SetAspectRatio(width, height);
    m_physicalSky.HandleReshapeEvent(width, height);
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

    switch (m_skyType)
    {
    case SkyType::HDR:
    {
        m_hdrSky.Render(m_camera);
    } break;
    case SkyType::PHYSICAL:
    {
        m_physicalSky.HandleRedisplayEvent();
    } break;
    default:
    {
        std::cout << "E: Unknown sky type selected." << std::endl;
    }
    }
}