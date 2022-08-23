#version 330 core

// m_ : Model coordinate system
// w_ : World coordinate system
// v_ : View coordinate system
// t_ : Tangent coordinate system
// e_ : Earth coordinate system (Earth centric coordinate space, analogous to world space shifted so that the earth center is at the origin)

layout(location = 0) in vec4 Pos;

uniform mat4 WorldFromView;
uniform mat4 ViewFromClip;

out vec3 w_ViewDir;

void main()
{
    w_ViewDir = (WorldFromView * vec4((ViewFromClip * Pos).xyz, 0.0)).xyz;
    gl_Position = Pos;
}