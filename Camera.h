#include <glm/glm.hpp>

class Camera
{
public:
    Camera();
    void OnRender();
    void OnMouseMovement(double xpos, double ypos);
    void OnMouseClick(int button, int action, int mods);
    const glm::vec3& GetRight() const { return m_right; }
    const glm::vec3& GetForward() const { return m_forward; }
private:
    glm::vec3 m_position;
    glm::vec3 m_right;
    glm::vec3 m_up;
    glm::vec3 m_forward;
    float m_azimuth;
    bool m_isRotating;
};