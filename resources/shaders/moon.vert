#version 330 core

layout (location = 0) in vec3 m_Pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;

void main()
{
    gl_Position = projection * view * model * vec4(m_Pos.xy, 0.0, 1.0);
    TexCoord = m_Pos.xy;
}