#include "Window.h"

#include <nfd.hpp>

#include <cstdlib>
#include <iostream>

int main()
{
    glfwSetErrorCallback([](int error_code, const char* description)
    {
        std::cerr << "[glfw] E(" << error_code << "): " << description << std::endl;
    });

    if (glfwInit() != GLFW_TRUE)
    {
        std::cerr << "[glfw] E: Could not be initialized." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    else if (NFD::Init() != NFD_OKAY)
    {
        std::cerr << "[NFD] E: Could not be initialized." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    else
    {
        Window window = Window();
        window.Init();
        window.MainLoop();
    }

    NFD::Quit();
    glfwTerminate();
}
