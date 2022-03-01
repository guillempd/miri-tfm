#include "Camera.h"

#include <glm/glm.hpp>

#include <GLAD/glad.h>

class Application
{
public:
    Application();
    ~Application();
    void Initialize();
    void OnRender();
    void OnCursorMovement(double xpos, double ypos);
    void OnMouseClick(int button, int action, int mods);
private:
    void LoadHdrImage();
    void LoadShaders();
    void LoadFullScreenQuad();
private:
    Camera m_camera;
    glm::vec2 m_previousCursorPosition;
    float m_clearColor[3];
    float m_verticalFov;
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_program;
    GLuint m_texture;
};