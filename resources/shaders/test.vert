#version 330 core

layout (location = 0) in vec3 m_Pos;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec2 TexCoord;

void main()
{
    TexCoord = (m_Pos.xy + 1.0) / 2.0;
    gl_Position = Projection * View * Model * vec4(m_Pos.xy, 0.0, 1.0);
}