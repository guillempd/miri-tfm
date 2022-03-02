#include "Window.h"

#include <GLFW/glfw3.h>

#include <nfd.hpp>

#include <iostream>

int main()
{
    glfwSetErrorCallback([](int error_code, const char* description)
    {
        std::cerr << "[glfw] E(" << error_code << "): " << description << std::endl;
    });

    if (glfwInit())
    {
        NFD::Init();
        Window window = Window();
        window.Initialize();
        window.MainLoop();
    }

    NFD::Quit();
    glfwTerminate();
}
