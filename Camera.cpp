#include "Camera.h"

#include <imgui.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>

#include <glm/matrix.hpp>

#include <GLFW/glfw3.h> // TODO: Remove and use Window.h instead

Camera::Camera()
    : m_position(0.0f,  0.0f,  0.0f)
    , m_right   (1.0f,  0.0f,  0.0f)
    , m_up      (0.0f,  1.0f,  0.0f)
    , m_forward (0.0f,  0.0f, -1.0f)
    , m_azimuth(glm::half_pi<float>())
    , m_zenith(glm::half_pi<float>())
    , m_isRotating(false)
    , m_verticalFov(glm::half_pi<float>())
    , m_radius(1.0f)
{
}

void Camera::OnRender()
{
    if (ImGui::Begin("Camera"))
    {
        ImGui::SliderFloat("Azimuth", &m_azimuth, 0.0f, glm::two_pi<float>());
        ImGui::SliderFloat("Vertical Fov", &m_verticalFov, glm::quarter_pi<float>(), glm::two_thirds<float>() * glm::pi<float>());
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
    m_position = -m_forward * m_radius;
}

void Camera::OnMouseMovement(glm::vec2 movement)
{
    if (!m_isRotating) return;

    // m_isRotating
    m_azimuth += 0.01f * movement.x;
    m_azimuth = glm::mod(m_azimuth, glm::two_pi<float>());

    constexpr float safetyMargin = 0.25f;
    m_zenith -= 0.01f * movement.y;
    m_zenith = glm::clamp(m_zenith, safetyMargin, glm::pi<float>() - safetyMargin);
}

void Camera::OnMouseClick(int button, int action, int mods)
{

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS) m_isRotating = true;
        else /* action == GLFW_RELEASE */ m_isRotating = false;
    }
}

void Camera::OnScroll(int movement)
{
    if (movement > 0) while (movement--) m_radius /= 1.1f;
    else while (movement++) m_radius *= 1.1f;
}

void Camera::SetAspectRatio(int width, int height)
{
    m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(m_position, glm::vec3(0.0f), m_up);
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    constexpr float zNear = 0.01f;
    constexpr float zFar = 100.0f;
    return glm::perspective(m_verticalFov, m_aspectRatio, 0.01f, 100.0f);
}
