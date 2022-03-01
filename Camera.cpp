#include "Camera.h"

#include <imgui.h>

#include <GLFW/glfw3.h>

Camera::Camera()
    : m_position(0.0f,  0.0f,  0.0f)
    , m_right   (1.0f,  0.0f,  0.0f)
    , m_up      (0.0f,  1.0f,  0.0f)
    , m_forward (0.0f,  0.0f, -1.0f)
    , m_azimuth(0.0f)
    , m_zenith(3.1415f / 2.0f)
    , m_isRotating(false)
{
}

void Camera::OnRender()
{
    /*if (ImGui::Begin("Camera"))
    {
        ImGui::SliderFloat("Azimuth", &m_azimuth, 0.0f, 2.0f * 3.14f);
    }
    ImGui::End();*/
    m_forward = glm::vec3(glm::sin(m_zenith) * glm::cos(m_azimuth), glm::cos(m_zenith), glm::sin(m_zenith) * glm::sin(m_azimuth));
    m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_up = glm::cross(m_right, m_forward);
}

void Camera::OnMouseMovement(glm::vec2 movement)
{
    if (!m_isRotating) return;

    // m_isRotating
    m_azimuth += 0.01f * movement.x;
    m_azimuth = glm::mod(m_azimuth, 2.0f * 3.1415f);

    m_zenith += 0.01f * movement.y;
    m_zenith = glm::clamp(m_zenith, 0.25f, 3.1415f - 0.25f);
}

void Camera::OnMouseClick(int button, int action, int mods)
{

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS) m_isRotating = true;
        else /* action == GLFW_RELEASE */ m_isRotating = false;
    }
}
