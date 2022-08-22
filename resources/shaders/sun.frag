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
    vec3 inscatter = GetSolarSkyRadiance(p_CameraPos, p_ViewDirection, 0.0, p_SunDirection, transmittance);
    vec3 radiance = GetSunRadiance();
    vec3 result = radiance * transmittance + inscatter;
    FragColor = vec4(result, 1.0);   
}

// LIMB DARKENING STUFF TO BE IMPLEMENTED
// uniform int limb_darkening_strategy;

// #define LIMB_DARKENING_NONE 0
// #define LIMB_DARKENING_NEC96 1
// #define LIMB_DARKENING_HM98 2
// #define LIMB_DARKENING_OTHER 3

// vec3 GetNEC96LimbDarkeningFactor(float centerToEdge)
// {
//   vec3 u = vec3(1.000, 1.000, 1.000);
//   vec3 a = vec3(0.397, 0.503, 0.652); // Coefficients for RGB wavelength (680, 550, 440)

//   // centerToEdge = 1.0 - centerToEdge;
//   // float mu = sqrt(1.0 - centerToEdge * centerToEdge);
//   float mu = centerToEdge;

//   vec3 factor = 1.0 - u * (1.0 - pow(vec3(mu), a));
//   return factor;
// }

// vec3 GetHM98LimbDarkeningFactor(float centerToEdge)
// {
//   // Coefficients for RGB wavelength (680, 550, 440)
//   vec3 a0 = vec3( 0.34685, 0.26073, 0.15248);
//   vec3 a1 = vec3( 1.37539, 1.27428, 1.38517);
//   vec3 a2 = vec3(-2.04425,-1.30352,-1.49615);
//   vec3 a3 = vec3( 2.70493, 1.47085, 1.99886);
//   vec3 a4 = vec3(-1.94290,-0.96618,-1.48155);
//   vec3 a5 = vec3( 0.55999, 0.26384, 0.44119);

//   // centerToEdge = 1.0 - centerToEdge;
//   // float mu = sqrt(1.0 - centerToEdge * centerToEdge);
//   float mu = centerToEdge;
//   float mu2 = mu * mu;
//   float mu3 = mu2 * mu;
//   float mu4 = mu2 * mu2;
//   float mu5 = mu4 * mu;

//   vec3 factor = a0 + a1*mu + a2*mu2 + a3*mu3 + a4*mu4 + a5*mu5;
//   return factor;
// }

// vec3 GetOtherLimbDarkeningFactor(float centerToEdge)
// {
//   vec3 u = vec3(0.9, 0.9, 0.9);
//   float mu = centerToEdge;
//   vec3 factor = 1.0 - u * (1.0 - mu);
//   return factor;
// }

// vec3 GetLimbDarkeningFactor(float centerToEdge)
// {
//   switch (limb_darkening_strategy)
//   {
//     case LIMB_DARKENING_NONE:   return vec3(1.0);
//     case LIMB_DARKENING_NEC96:  return GetNEC96LimbDarkeningFactor(centerToEdge);
//     case LIMB_DARKENING_HM98:   return GetHM98LimbDarkeningFactor(centerToEdge);
//     case LIMB_DARKENING_OTHER:  return GetOtherLimbDarkeningFactor(centerToEdge);
//     default:                    return vec3(1.0, 0.0, 1.0);
//   }
// }