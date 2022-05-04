#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
    Camera();
    void OnUpdate();
    bool OnMouseMovement(glm::vec2 movement);
    void OnMouseClick(int button, int action, int mods);
    void OnScroll(int movement);
    const glm::vec3& GetPosition() const { return m_position; }
    const glm::vec3& GetRight() const { return m_right; }
    const glm::vec3& GetForward() const { return m_forward; }
    const glm::vec3& GetUp() const { return m_up; }
    float GetVerticalFov() const { return m_verticalFov; }
    float GetAspectRatio() const { return m_aspectRatio; }
    void SetAspectRatio(float aspectRatio);
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetViewFromClipMatrix() const;
    glm::mat4 GetWorldFromViewMatrix() const;
    void Reset();
private:
    glm::vec3 m_position;
    glm::vec3 m_right;
    glm::vec3 m_up;
    glm::vec3 m_forward;
    glm::vec3 m_center;
    float m_azimuth;
    float m_zenith;
    bool m_isRotating;
    bool m_isPanning;
    float m_verticalFov;
    float m_radius;
    float m_aspectRatio;
};