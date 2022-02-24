cmake_minimum_required(VERSION 3.18)

project(miri-tfm)

add_executable(miri-tfm main.cpp)

add_subdirectory(external/glfw)
add_subdirectory(external/glad)

target_include_directories(miri-tfm
    PRIVATE external/glfw/include
    PRIVATE external/glad/include
)

target_link_libraries(miri-tfm
    glfw
    glad
)
