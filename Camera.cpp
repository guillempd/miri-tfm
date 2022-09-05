#include "Camera.h"

#include <imgui.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>

#include <GLFW/glfw3.h> // NOTE: Remove and use Window.h instead

Camera::Camera()
    : m_position(0.0f,  0.0f,  0.0f)
    , m_right   (1.0f,  0.0f,  0.0f)
    , m_up      (0.0f,  1.0f,  0.0f)
    , m_forward (0.0f,  0.0f, -1.0f)
    , m_center  (0.0f,  0.0f,  0.0f)
    , m_azimuth(glm::half_pi<float>())
    , m_zenith(glm::half_pi<float>())
    , m_isRotating(false)
    , m_isPanning(false)
    , m_verticalFov(glm::half_pi<float>())
    , m_radius(1e-3f)
    , m_aspectRatio(1.0f)
{

}

void Camera::OnUpdate()
{
    if (ImGui::Begin("Camera"))
    {
        ImGui::SliderFloat("Azimuth", &m_azimuth, 0.0f, glm::two_pi<float>());
        ImGui::SliderFloat("Zenith", &m_zenith, 0.0f, glm::pi<float>());
        ImGui::SliderFloat("Vertical Fov", &m_verticalFov, glm::quarter_pi<float>(), glm::two_thirds<float>() * glm::pi<float>());
        if (ImGui::Button("Reset")) Reset();
    }
    ImGui::End();

    float cos_z = glm::cos(m_zenith);
    float sin_z = glm::sin(m_zenith);
    float cos_a = glm::cos(m_azimuth);
    float sin_a = glm::sin(m_azimuth);
    m_forward = -glm::vec3(sin_z * cos_a, cos_z, sin_z * sin_a);
    m_right = glm::vec3(sin_a, 0.0f, -cos_a);
    m_up = glm::vec3(-cos_z * cos_a, sin_z, -cos_z * sin_a);
    m_position = m_center - m_forward * m_radius;
}

bool Camera::OnCursorMovement(glm::vec2 movement)
{
    constexpr float sensitivity = 0.01f;
    constexpr float safetyMargin = 0.25f;
    if (m_isRotating)
    {
        m_azimuth += sensitivity * movement.x;
        m_azimuth = glm::mod(m_azimuth, glm::two_pi<float>());

        m_zenith -= sensitivity * movement.y;
        m_zenith = glm::clamp(m_zenith, safetyMargin, glm::pi<float>() - safetyMargin);
        return true;
    }
    if (m_isPanning)
    {
        m_center += sensitivity * (movement.x * (-m_right) + movement.y * m_up);
        return true;
    }
    return false;
}

void Camera::OnMouseButton(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS) m_isRotating = true;
        else /* action == GLFW_RELEASE */ m_isRotating = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        if (action == GLFW_PRESS) m_isPanning = true;
        else /* action == GLFW_RELEASE */ m_isPanning = false;
    }
}

void Camera::OnScroll(int movement)
{
    if (movement > 0) while (movement--) m_radius /= 1.1f;
    else while (movement++) m_radius *= 1.1f;
}

void Camera::SetAspectRatio(float aspectRatio)
{
    m_aspectRatio = aspectRatio;
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(m_position, m_center, m_up);
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    constexpr float zNear = 1e-4f;
    constexpr float zFar = 1e3f;
    return glm::perspective(m_verticalFov, m_aspectRatio, zNear, zFar);
}

glm::mat4 Camera::GetViewFromClipMatrix() const
{
    float tanFov = glm::tan(m_verticalFov / 2.0f);
    return glm::mat4(glm::vec4(tanFov * m_aspectRatio, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, tanFov, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));
}

glm::mat4 Camera::GetWorldFromViewMatrix() const
{
    return glm::mat4(glm::vec4(m_right, 0.0f), glm::vec4(m_up, 0.0f), glm::vec4(-m_forward, 0.0f), glm::vec4(m_position, 1.0f));
}

void Camera::Reset()
{
    m_azimuth = glm::half_pi<float>();
    m_zenith = glm::half_pi<float>();
    m_center = glm::vec3(0.0f, 0.0f, 0.0f);
    m_radius = 1.0f;
    m_verticalFov = glm::half_pi<float>();
}