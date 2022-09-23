#pragma once

#include "Camera.h"
#include "PhysicalSky.h"
class Window;
#include "ShaderProgram.h"

#include <glm/glm.hpp>

#include <memory>

class Application
{
public:
    Application(int width, int height, Window* window);
    ~Application();
    void OnUpdate();
    void OnRender();
    void OnCursorPos(double xpos, double ypos);
    void OnMouseButton(int button, int action, int mods);
    void OnScroll(double xoffset, double yoffset);
    void OnFramebufferSize(int width, int height);
private:
    enum class DisplayMode {DAY, NIGHT, PHOTOPIC_LUMINANCE, SCOTOPIC_LUMINANCE};
private:
    Camera m_camera;
    glm::vec2 m_previousCursorPosition;
    PhysicalSky m_physicalSky;
    Window* m_window;

    // POSTPROCESS STUFF
    GLuint m_hdrFramebuffer;
    GLuint m_hdrTexture;
    GLuint m_depthRenderbuffer;
    ShaderProgram m_postprocessShader;
    GLuint m_fullScreenQuadVao;
    GLuint m_fullScreenQuadVbo;
    float m_exposure;
    float m_max_white;
    DisplayMode m_displayMode;
    glm::vec3 m_tintColor;
};