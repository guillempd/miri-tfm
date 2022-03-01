#include "Application.h"

#include <imgui.h>

#include <iostream>

Application::Application()
    : m_camera()
    , m_previousCursorPosition()
    , m_clearColor{1.0f, 1.0f, 1.0f}
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

    if (ImGui::Begin("Color picker"))
    {
        ImGui::ColorEdit3("Clear Color", m_clearColor);
    }
    ImGui::End();

    m_camera.OnRender();

    glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_hdrSky.Render(m_camera);
}