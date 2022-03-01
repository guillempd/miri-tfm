cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(miri-tfm)

add_executable(miri-tfm
    main.cpp
    Application.cpp
    Window.cpp
    Camera.cpp
    Texture.cpp
    Program.cpp
    HdrSky.cpp
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

add_subdirectory(external/glfw)
add_subdirectory(external/glad)

target_include_directories(miri-tfm
    PRIVATE external/glfw/include
    PRIVATE external/glad/include
    PRIVATE external/imgui
    PRIVATE external/imgui/backends
    PRIVATE external/stb
    PRIVATE external/glm
)

target_link_libraries(miri-tfm
    glfw
    glad
)
