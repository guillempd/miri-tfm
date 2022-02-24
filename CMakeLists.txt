cmake_minimum_required(VERSION 3.18)

project(miri-tfm)

add_executable(miri-tfm main.cpp)

add_subdirectory(external/glfw)

target_include_directories(miri-tfm
    PRIVATE external/glfw/include
)

target_link_libraries(miri-tfm
    glfw
)
