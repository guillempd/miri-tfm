#version 330 core

layout (location = 0) in vec3 m_Pos;

out vec3 FragPosition;

void main()
{
    gl_Position = vec4(m_Pos, 1.0);
    FragPosition = m_Pos;
}