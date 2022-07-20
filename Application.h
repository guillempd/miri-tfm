#pragma once

#include "Camera.h"
//#include "HdrSky.h"
#include "PhysicalSky.h"
#include "Mesh.h"
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
    enum class SkyType {HDR, PHYSICAL};
private:
    Camera m_camera;
    glm::vec2 m_previousCursorPosition;
    // HdrSky m_hdrSky;
    PhysicalSky m_physicalSky;
    SkyType m_skyType;
    Window* m_window;
    //std::unique_ptr<Mesh> m_mesh;

    // POSTPROCESS STUFF
    GLuint m_hdrFramebuffer;
    GLuint m_hdrTexture;
    ShaderProgram m_postprocessShader;
    GLuint m_fullScreenQuadVao;
    GLuint m_fullScreenQuadVbo;
};