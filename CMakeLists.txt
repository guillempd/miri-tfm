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
    ShaderProgram.cpp
    ShaderSource.cpp
    HdrSky.cpp
    PhysicalSky.cpp
    Mesh.cpp
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
    external/precomputed_atmospheric_scattering/atmosphere/model.cc
)

add_subdirectory(external/glfw)
add_subdirectory(external/glad)
add_subdirectory(external/nativefiledialog-extended)
add_subdirectory(external/assimp)

target_include_directories(miri-tfm
    PRIVATE external/glfw/include
    PRIVATE external/glad/include
    PRIVATE external/nativefiledialog-extended/src/include
    PRIVATE external/imgui
    PRIVATE external/imgui/backends
    PRIVATE external/stb
    PRIVATE external/glm
    PRIVATE external/precomputed_atmospheric_scattering
    PRIVATE external/assimp/include
)

target_link_libraries(miri-tfm
    glfw
    glad
    nfd
    assimp
)
