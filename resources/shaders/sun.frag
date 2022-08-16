#version 330 core
#include "atmosphere.glsl"

in vec3 w_Pos;
in vec2 TexCoord;

// m_ : Model Space
// w_ : World Space
// v_ : View Space
// t_ : Tangent Space
// p_ : Planet Space (World space shifted so that planet is at origin)

uniform vec3 w_CameraPos;
uniform vec3 w_PlanetPos;
uniform vec3 w_SunDirection;

out vec4 FragColor;

void main()
{
    if (length(TexCoord) > 1.0) discard;

    vec3 p_CameraPos = w_CameraPos - w_PlanetPos;
    vec3 p_Pos = w_Pos - w_PlanetPos;
    vec3 p_ViewDirection = normalize(p_Pos - p_CameraPos);
    vec3 p_SunDirection = w_SunDirection;
    vec3 transmittance;
    vec3 inscatter = GetSunSkyRadiance(p_CameraPos, p_ViewDirection, 0.0, p_SunDirection, transmittance);
    vec3 radiance = GetSunRadiance();
    vec3 result = radiance * transmittance + inscatter;
    FragColor = vec4(result, 1.0);   
}