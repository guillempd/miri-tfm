#include "Application.h"

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include <glad/glad.h>

#include <iostream>

Application::Application()
    : m_camera()
    , m_previousCursorPosition()
    , m_hdrSky()
{
    std::cout << "Creating Application" << std::endl;
}

Application::~Application()
{
    std::cout << "Destroying Application" << std::endl;
}

void Application::Initialize()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_hdrSky.Load();
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

void Application::OnRender()
{
    ImGui::ShowDemoWindow();

    m_camera.OnRender();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_hdrSky.Render(m_camera);
}