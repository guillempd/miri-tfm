#include "Application.h"

#include <glm/gtc/type_ptr.hpp>

#include <nfd.hpp>

#include <imgui.h>

#include <glad/glad.h>

#include <iostream>

Application::Application(int width, int height, Window* window)
    : m_camera()
    , m_previousCursorPosition()
    , m_hdrSky()
    , m_physicalSky()
    , m_skyType(SkyType::HDR)
    , m_window(window)
    , m_mesh()
{
    std::cout << "Creating application" << std::endl;

    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    m_hdrSky.Load();
    m_physicalSky.Initialize(m_window);
    m_mesh = std::make_unique<Mesh>();
    OnFramebufferSize(width, height);
}

Application::~Application()
{
    std::cout << "Destroying Application" << std::endl;
}

void Application::OnCursorMovement(double xpos, double ypos)
{
    glm::vec2 currentCursorPosition = glm::vec2(xpos, ypos);
    glm::vec2 cursorMovement = currentCursorPosition - m_previousCursorPosition;
    bool captured = false;
    if (!captured) captured = m_physicalSky.OnMouseMovement(cursorMovement);
    if (!captured) captured = m_camera.OnMouseMovement(cursorMovement);

    m_previousCursorPosition = currentCursorPosition;
}

void Application::OnMouseClick(int button, int action, int mods)
{
    m_camera.OnMouseClick(button, action, mods);
    m_physicalSky.OnMouseClick(button, action, mods);
}

void Application::OnFramebufferSize(int width, int height)
{
    glViewport(0, 0, width, height);
    m_camera.SetAspectRatio(width, height);
    GLenum errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) std::cerr << "GL error after resize" << std::endl;
}

void Application::OnScroll(double xoffset, double yoffset)
{
    m_camera.OnScroll(static_cast<int>(yoffset));
}

void Application::OnRender()
{
    ImGui::ShowDemoWindow();
    m_camera.OnRender();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (ImGui::Begin("Sky Selection"))
    {
        ImGui::RadioButton("HDR Sky", reinterpret_cast<int*>(&m_skyType), static_cast<int>(SkyType::HDR));
        ImGui::RadioButton("Physical Sky", reinterpret_cast<int*>(&m_skyType), static_cast<int>(SkyType::PHYSICAL));
    }
    ImGui::End();

    if (ImGui::Begin("Model"))
    {
        if (ImGui::Button("Open other model..."))
        {
            NFD::UniquePath path;
            NFD::OpenDialog(path);
            if (path)
            {
                std::cout << "Opened file: " << path << std::endl;
                m_mesh = std::make_unique<Mesh>(path.get());
            }
        }
        static glm::vec4 albedo = glm::vec4(1.0);
        ImGui::ColorEdit4("Albedo", glm::value_ptr(albedo), 0);
        m_mesh->SetAlbedo(albedo);
        if (ImGui::Button("Load albedo texture..."))
        {
            NFD::UniquePath path;
            NFD::OpenDialog(path);
            if (path)
            {
                std::cout << "Opened file: " << path << std::endl;
                m_mesh->LoadAlbedoTexture(path.get());
            }
        }

        if (ImGui::Button("Load normal texture..."))
        {
            NFD::UniquePath path;
            NFD::OpenDialog(path);
            if (path)
            {
                std::cout << "Opened file: " << path << std::endl;
                m_mesh->LoadNormalTexture(path.get());
            }
        }
    }
    ImGui::End();

    switch (m_skyType)
    {
    case SkyType::HDR:
    {
        m_hdrSky.Render(m_camera);
        m_mesh->Render(m_camera);
        GLenum errorCode = glGetError();
        if (errorCode != GL_NO_ERROR) std::cerr << "GL error after rendering HDR Sky" << std::endl;
    } break;
    case SkyType::PHYSICAL:
    {
        m_physicalSky.Render(m_camera);
        m_mesh->JustRender(m_camera);
        GLenum errorCode = glGetError();
        if (errorCode != GL_NO_ERROR) std::cerr << "GL error after rendering physical sky" << std::endl;
    } break;
    default:
    {
        std::cout << "E: Unknown sky type selected." << std::endl;
    }
    }
}