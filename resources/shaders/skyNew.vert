#version 330 core

layout(location = 0) in vec4 vertex;

uniform mat4 model_from_view;
uniform mat4 view_from_clip;

out vec3 view_ray;

void main()
{
    view_ray = (model_from_view * vec4((view_from_clip * vec4(vertex.xy, 1.0, 0.0)).xyz, 0.0)).xyz;
    gl_Position = vertex;
}