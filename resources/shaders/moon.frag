#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

void main()
{
    if (length(TexCoord) > 1.0) discard;
    vec3 color = 1000.0 * vec3(TexCoord, 0.0);
    FragColor = vec4(vec3(1000.0), 1.0);
}