#pragma once

#include "Camera.h"
#include "HdrSky.h"

#include <glm/glm.hpp>

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
    Camera m_camera;
    glm::vec2 m_previousCursorPosition;
    HdrSky m_hdrSky;
};