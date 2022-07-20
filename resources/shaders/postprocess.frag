#version 330 core

in vec3 FragPosition;

out vec4 FragColor;

void main()
{
    FragColor.rgb = FragPosition;
    FragColor.a = 1.0;
}