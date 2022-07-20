#version 330 core

layout (location = 0) in vec3 m_Pos;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(m_Pos, 1.0);
    TexCoord = (m_Pos.xy + 1.0) / 2.0;
}