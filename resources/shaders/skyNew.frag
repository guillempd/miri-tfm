#version 330 core
#include "atmosphere.glsl"

in vec3 view_ray;

uniform vec3 camera;
uniform vec3 earth_center;
uniform vec3 sun_direction;
uniform vec3 sun_size;

out vec4 FragColor;

void main()
{
    vec3 view_direction = normalize(view_ray);
    vec3 transmittance;
    vec3 radiance = GetSkyRadiance(camera - earth_center, view_direction, 0.0, sun_direction, transmittance);
    if (dot(view_direction, sun_direction) > sun_size.y) {
        radiance = radiance + transmittance * GetSolarRadiance();
    }
    // FragColor.rgb = pow(vec3(1.0) - exp(-radiance / white_point * exposure), vec3(1.0 / 2.2));
    FragColor.rgb = radiance * 0.000025;
    FragColor.a = 1.0;
}