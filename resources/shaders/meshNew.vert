#version 330 core
// m_ : Model Space
// w_ : World Space
// v_ : View Space
// t_ : Tangent Space

layout (location = 0) in vec3 m_Pos;
layout (location = 1) in vec3 m_Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 w_LightDir;

out vec3 v_LightDir;

out vec3 v_Normal;

void main()
{
    gl_Position = projection * view * model * vec4(m_Pos, 1.0);
    v_LightDir = (view * vec4(w_LightDir, 0.0)).xyz;
    v_Normal = mat3(inverse(transpose(view * model))) * m_Normal;
}