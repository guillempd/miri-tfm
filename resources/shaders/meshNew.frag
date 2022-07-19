#version 330 core
#include "atmosphere.glsl"

// m_ : Model Space
// w_ : World Space
// v_ : View Space
// t_ : Tangent Space
// p_ : Planet Space (World space shifted so that planet is at origin)

uniform vec3 w_LightDir;
uniform vec3 w_CameraPos;
uniform vec3 w_PlanetPos;

in vec3 w_Pos;
in vec3 v_LightDir;
in vec3 v_Normal;

out vec4 FragColor;

void main()
{
    vec3 L = normalize(v_LightDir);
    vec3 N = normalize(v_Normal);
    vec3 color = vec3(0.3, 0.3, 0.3);

    vec3 ambientStrength = vec3(0.3);
    vec3 ambientTerm = color * ambientStrength;

    vec3 diffuseStrength = vec3(0.7);
    float diffuseFactor = max(dot(N, L), 0.0);
    vec3 diffuseTerm = color * diffuseStrength * diffuseFactor;

    vec3 specularTerm = vec3(0.0);

    vec3 result = ambientTerm + diffuseTerm + specularTerm;

    // APPLY TRANSMITTANCE


    vec3 transmittance;
    vec3 sky_irradiance;
    vec3 p_CameraPos = w_CameraPos - w_PlanetPos;
    vec3 p_Pos = w_Pos - w_PlanetPos;
    vec3 p_LightDir = w_LightDir;

    vec3 sun_irradiance = GetSunAndSkyIrradiance(p_Pos, vec3(0.0, 1.0, 0.0), p_LightDir, sky_irradiance);
    vec3 radiance = GetSkyRadianceToPoint(p_CameraPos, p_Pos, 0.0, p_LightDir, transmittance);
    result = result * transmittance + radiance * 0.0001;

    vec3 alt_result = color * (sun_irradiance + sky_irradiance);
    // alt_result = alt_result * transmittance + radiance;

    FragColor = vec4(alt_result * 0.00001, 1.0);
}