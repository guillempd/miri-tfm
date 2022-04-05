#version 330 core
#include "include.glsl"

#inject

void main()
{
    vec3 position = includedFunction();
}