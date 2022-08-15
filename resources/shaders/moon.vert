#version 330 core

layout (location = 0) in vec3 m_Pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 w_Pos;
out vec2 TexCoord;

void main()
{
    w_Pos = (model * vec4(m_Pos.xy, 0.0, 1.0)).xyz;
    gl_Position = projection * view * vec4(w_Pos, 1.0);
    TexCoord = m_Pos.xy;
}