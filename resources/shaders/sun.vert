#version 330 core

layout (location = 0) in vec3 m_Pos;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec2 TexCoord;
out vec3 w_Pos;

void main()
{
    TexCoord = m_Pos.xy;
    w_Pos = (Model * vec4(m_Pos.xy, 0.0, 1.0)).xyz;
    gl_Position = Projection * View * vec4(w_Pos, 1.0);
}