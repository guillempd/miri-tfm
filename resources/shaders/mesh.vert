#version 330 core

layout (location = 0) in vec3 m_Pos;
layout (location = 1) in vec3 m_Normal;
layout (location = 2) in vec2 m_TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 albedo;

out vec3 v_Normal;
out vec2 t_TexCoord;

void main()
{
    gl_Position = projection * view * model * vec4(m_Pos, 1.0);
    v_Normal = inverse(transpose(mat3(view * model))) * m_Normal;
    t_TexCoord = m_TexCoord;
}