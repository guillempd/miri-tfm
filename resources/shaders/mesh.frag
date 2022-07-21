#version 330 core
#include "atmosphere.glsl"
#define PI 3.14159265

// m_ : Model Space
// w_ : World Space
// v_ : View Space
// t_ : Tangent Space
// p_ : Planet Space (World space shifted so that planet is at origin)

uniform vec3 w_LightDir;
uniform vec3 w_CameraPos;
uniform vec3 w_PlanetPos;
uniform vec3 albedo;

in vec3 w_Pos;
in vec3 v_LightDir;
in vec3 v_Normal;
in vec3 w_Normal;

out vec4 FragColor;

void main()
{
    vec3 p_CameraPos = w_CameraPos - w_PlanetPos;
    vec3 p_Pos = w_Pos - w_PlanetPos;
    vec3 p_LightDir = w_LightDir;
    vec3 p_Normal = normalize(w_Normal);

    vec3 sky_irradiance;
    vec3 sun_irradiance = GetSunAndSkyIrradiance(p_Pos, p_Normal, p_LightDir, sky_irradiance);
    vec3 transmittance;
    vec3 radiance = GetSkyRadianceToPoint(p_CameraPos, p_Pos, 0.0, p_LightDir, transmittance);
    vec3 result = albedo * (1 / PI) * (sky_irradiance + sun_irradiance) * transmittance + radiance;
    FragColor = vec4(result, 1.0);
}