#include "Camera.h"

#include <imgui.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <glm/matrix.hpp>

#include <GLFW/glfw3.h>

Camera::Camera()
    : m_position(0.0f,  0.0f,  0.0f)
    , m_right   (1.0f,  0.0f,  0.0f)
    , m_up      (0.0f,  1.0f,  0.0f)
    , m_forward (0.0f,  0.0f, -1.0f)
    , m_azimuth(0.0f)
    , m_zenith(3.1415f / 2.0f)
    , m_isRotating(false)
    , m_verticalFov(3.1415f / 4.0f)
{
}

void Camera::OnRender()
{
    if (ImGui::Begin("Camera"))
    {
        ImGui::SliderFloat("Azimuth", &m_azimuth, 0.0f, 2.0f * 3.14f);
        ImGui::SliderFloat("Vertical Fov", &m_verticalFov, 3.1415f / 8.0f, 3.1415f / 2.0f);
    }
    ImGui::End();

    /*m_forward = glm::vec3(glm::sin(m_zenith) * glm::cos(m_azimuth), glm::cos(m_zenith), glm::sin(m_zenith) * glm::sin(m_azimuth));
    m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_up = glm::cross(m_right, m_forward);*/

    // ORBIT CONTROLS
    float cos_z = glm::cos(m_zenith);
    float sin_z = glm::sin(m_zenith);
    float cos_a = glm::cos(m_azimuth);
    float sin_a = glm::sin(m_azimuth);
    m_forward = -glm::vec3(sin_z * cos_a, cos_z, sin_z * sin_a);
    m_right = glm::vec3(sin_a, 0.0f, -cos_a);
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

void Camera::SetAspectRatio(int width, int height)
{
    m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(-m_forward, glm::vec3(0.0f), m_up);
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    return glm::perspective(m_verticalFov, m_aspectRatio, 0.01f, 100.0f);
}
