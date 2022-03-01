#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
    Camera();
    void OnRender();
    void OnMouseMovement(glm::vec2 movement);
    void OnMouseClick(int button, int action, int mods);
    const glm::vec3& GetRight() const { return m_right; }
    const glm::vec3& GetForward() const { return m_forward; }
    const glm::vec3& GetUp() const { return m_up; }
    float GetVerticalFov() const { return m_verticalFov; }
private:
    glm::vec3 m_position;
    glm::vec3 m_right;
    glm::vec3 m_up;
    glm::vec3 m_forward;
    float m_azimuth;
    float m_zenith;
    bool m_isRotating;
    float m_verticalFov;
};