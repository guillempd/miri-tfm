#include <GLAD/glad.h>

class Application
{
public:
    Application();
    ~Application();
    void Init();
    void OnRender();
    void OnCursorMovement(double xpos, double ypos);
private:
    void LoadHdrImage();
    void LoadShaders();
    void LoadFullScreenQuad();
private:
    float m_clearColor[3];
    float m_verticalFov;
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_program;
    GLuint m_texture;
};