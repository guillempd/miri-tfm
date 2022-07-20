#version 330 core

in vec3 FragPosition;

uniform sampler2D hdrTexture;

out vec4 FragColor;

void main()
{
    FragColor = 1.0 - texture(hdrTexture, FragPosition.st);
    FragColor.a = 1.0;
}