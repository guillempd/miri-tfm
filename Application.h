#pragma once

#include "Camera.h"
#include "HdrSky.h"

#include "atmosphere/demo/demo.h"

#include <glm/glm.hpp>

class Application
{
public:
    Application();
    ~Application();
    void Initialize(int width, int height);
    void OnRender();
    void OnCursorMovement(double xpos, double ypos);
    void OnMouseClick(int button, int action, int mods);
    void OnFramebufferSize(int width, int height);
private:
    enum class SkyType {HDR, PHYSICAL};
private:
    Camera m_camera;
    glm::vec2 m_previousCursorPosition;
    HdrSky m_hdrSky;
    atmosphere::demo::Demo m_physicalSky;
    SkyType m_skyType;
private:
};