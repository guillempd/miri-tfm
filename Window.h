#include "Application.h"

#include <GLFW/glfw3.h>

class Window
{
public:
    Window();
    ~Window();
    void Initialize();
    void MainLoop();
private:
    void OnMouseButton(int button, int action, int mods);
    void OnCursorPos(double xpos, double ypos);
    void OnFramebufferSize(int width, int height);
    void MakeCurrent() const;
    void InstallCallbacks();
    void InitializeImGui() const;
private:
    GLFWwindow* m_window;
    Application m_application;
};