#version 330 core

// m_ : Model coordinate system
// w_ : World coordinate system
// v_ : View coordinate system
// t_ : Tangent coordinate system
// e_ : Earth coordinate system (Earth centric coordinate space, analogous to world space shifted so that the earth center is at the origin, this is the one that has to be used for atmospheric functions)

layout (location = 0) in vec3 m_Pos;
layout (location = 1) in vec3 m_Normal;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 w_Pos;
out vec3 w_Normal;

void main()
{
    w_Pos = (Model * vec4(m_Pos, 1.0)).xyz;
    w_Normal = inverse(transpose(mat3(Model))) * m_Normal;
    gl_Position = Projection * View * vec4(w_Pos, 1.0);
}