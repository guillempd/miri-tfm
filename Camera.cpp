#include "Camera.h"

#include  <imgui.h>

// #include <GLFW/glfw3.h>

Camera::Camera()
    : m_position(0.0f,  0.0f,  0.0f)
    , m_right   (1.0f,  0.0f,  0.0f)
    , m_up      (0.0f,  1.0f,  0.0f)
    , m_forward (0.0f,  0.0f, -1.0f)
    , m_azimuth(0.0f)
    , m_isRotating(false)
{
}

void Camera::OnRender()
{
    if (ImGui::Begin("Camera"))
    {
        ImGui::SliderFloat("Azimuth", &m_azimuth, 0.0f, 2.0f * 3.14f);
    }
    ImGui::End();
    m_right = glm::vec3(glm::cos(m_azimuth), 0.0f, glm::sin(m_azimuth));
    m_forward = glm::cross(m_up, m_right);
}

void Camera::OnMouseMovement(double xpos, double ypos)
{
    // if (!m_isRotating) return;

    // m_isRotating
    
}

void Camera::OnMouseClick(int button, int action, int mods)
{
    /*
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;

    // button == GLFW_MOUSE_BUTTON_LEFT
    if (action == GLFW_PRESS) m_isRotating = true;
    else /* action == GLFW_RELEASE / m_isRotating = false;
    */
}
