#pragma once

#include "Camera.h"
#include "HdrSky.h"
#include "PhysicalSky.h"
class Window; // Only forward declaration
#include "Mesh.h"


#include <glm/glm.hpp>

class Application
{
public:
    Application(int width, int height, Window* window);
    ~Application();
    void OnRender();
    void OnCursorMovement(double xpos, double ypos);
    void OnMouseClick(int button, int action, int mods);
    void OnFramebufferSize(int width, int height);
    void OnScroll(double xoffset, double yoffset);
private:
    enum class SkyType {HDR, PHYSICAL};
private:
    Camera m_camera;
    glm::vec2 m_previousCursorPosition;
    HdrSky m_hdrSky;
    PhysicalSky m_physicalSky;
    SkyType m_skyType;
    Window* m_window;
    Mesh m_mesh;
};