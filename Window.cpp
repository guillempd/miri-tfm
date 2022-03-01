#include "Window.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>

Window::Window()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_window = glfwCreateWindow(1024, 768, "miri-tfm", nullptr, nullptr);
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
}

void Window::Initialize()
{
    MakeCurrent();
    InstallCallbacks();
    InitializeImGui();
    m_application.Initialize();
}

void Window::MakeCurrent() const
{
    glfwMakeContextCurrent(m_window);
    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
    glViewport(0, 0, 1024, 768);
}

void Window::InstallCallbacks()
{
    glfwSetWindowUserPointer(m_window, this);

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
    {
        Window* thisWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        thisWindow->OnMouseButton(button, action, mods);
    });

    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos)
    {
        Window* thisWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        thisWindow->OnCursorPos(xpos, ypos);
    });
}

void Window::InitializeImGui() const
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
}

void Window::MainLoop()
{
    while (!glfwWindowShouldClose(m_window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glfwPollEvents();
        m_application.OnRender();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(m_window);
    }
}

void Window::OnMouseButton(int button, int action, int mods)
{
    if (ImGui::GetIO().WantCaptureMouse) return;

}

void Window::OnCursorPos(double xpos, double ypos)
{
    if (ImGui::GetIO().WantCaptureMouse) return;

    m_application.OnCursorMovement(xpos, ypos);
}