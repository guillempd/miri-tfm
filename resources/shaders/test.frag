#version 330 core

in vec2 TexCoord;

uniform sampler2D AlbedoMap;

out vec4 Color;

void main()
{
    vec3 color = texture(AlbedoMap, TexCoord).rgb;
    color *= 100.0;
    Color = vec4(color, 1.0);
}