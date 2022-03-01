#include "Window.h"

#include <GLFW/glfw3.h>

#include <iostream>

int main()
{
    glfwSetErrorCallback([](int error_code, const char* description)
    {
        std::cerr << "[glfw] E(" << error_code << "): " << description << std::endl;
    });

    if (glfwInit())
    {
        Window window = Window();
        window.Initialize();
        window.MainLoop();
    }
    
    glfwTerminate();
}
