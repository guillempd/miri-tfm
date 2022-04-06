#version 330 core

layout (location = 0) in vec3 m_Pos;
layout (location = 1) in vec3 m_Normal;
layout (location = 2) in vec3 m_Tangent;
layout (location = 3) in vec3 m_Bitangent;
layout (location = 4) in vec2 m_TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 albedo;

out vec3 w_Normal;
out vec3 w_Tangent;
out vec3 w_Bitangent;
out vec2 t_TexCoord;

void main()
{
    gl_Position = projection * view * model * vec4(m_Pos, 1.0);
    w_Normal = inverse(transpose(mat3(model))) * m_Normal;
    w_Tangent = mat3(model) * m_Tangent;
    w_Bitangent = mat3(model) * m_Bitangent;
    t_TexCoord = m_TexCoord;
}