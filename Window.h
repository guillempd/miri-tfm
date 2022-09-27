#pragma once

#include "Application.h"

#include <GLFW/glfw3.h>

#include <memory>

class Window
{
public:
    Window();
    ~Window();
    void Init();
    void MainLoop();
    float GetTime() const;
private:
    void MakeCurrent() const;
    void InstallCallbacks();
    void InitImGui() const;
    void OnCursorPos(double xpos, double ypos);
    void OnMouseButton(int button, int action, int mods);
    void OnScroll(double xoffset, double yoffset);
    void OnFramebufferSize(int width, int height);
private:
    GLFWwindow* m_window;
    std::unique_ptr<Application> m_application;
};