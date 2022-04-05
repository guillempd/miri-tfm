#version 330 core
layout (location = 0) in vec2 aPos;
uniform mat4 view_from_clip;
uniform mat4 world_from_view;
out vec3 view_ray;
void main()
{
    view_ray = (world_from_view * view_from_clip * vec4(aPos.x, aPos.y, 1.0, 0.0)).xyz;
    gl_Position = vec4(aPos.x, aPos.y, 1.0, 1.0);
}