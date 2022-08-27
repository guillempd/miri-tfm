#version 330 core
#include "atmosphere.glsl"

// m_ : Model coordinate system
// w_ : World coordinate system
// v_ : View coordinate system
// t_ : Tangent coordinate system
// e_ : Earth coordinate system (Earth centric coordinate space, analogous to world space shifted so that the earth center is at the origin)

in vec2 TexCoord;
in vec3 w_Pos;

uniform vec3 w_CameraPos;
uniform vec3 w_EarthCenterPos;
uniform vec3 w_SunDir;
uniform vec3 w_MoonDir;

#define LIMB_DARKENING_NONE 0
#define LIMB_DARKENING_NEC96 1
#define LIMB_DARKENING_HM98 2
uniform int LimbDarkeningAlgorithm;

out vec4 Color;

vec3 GetNEC96LimbDarkeningFactor(float distSquared)
{
  vec3 u = vec3(1.000, 1.000, 1.000);
  vec3 a = vec3(0.397, 0.503, 0.652); // Coefficients for RGB wavelength (680, 550, 440)

  float mu = sqrt(1.0 - distSquared);

  vec3 factor = 1.0 - u * (1.0 - pow(vec3(mu), a));
  return factor;
}

vec3 GetHM98LimbDarkeningFactor(float distSquared)
{
  // Coefficients for RGB wavelength (680, 550, 440)
  vec3 a0 = vec3( 0.34685, 0.26073, 0.15248);
  vec3 a1 = vec3( 1.37539, 1.27428, 1.38517);
  vec3 a2 = vec3(-2.04425,-1.30352,-1.49615);
  vec3 a3 = vec3( 2.70493, 1.47085, 1.99886);
  vec3 a4 = vec3(-1.94290,-0.96618,-1.48155);
  vec3 a5 = vec3( 0.55999, 0.26384, 0.44119);

  float mu = sqrt(1.0 - distSquared);
  float mu2 = mu * mu;
  float mu3 = mu2 * mu;
  float mu4 = mu3 * mu;
  float mu5 = mu4 * mu;

  vec3 factor = a0 + a1*mu + a2*mu2 + a3*mu3 + a4*mu4 + a5*mu5;
  return factor;
}

vec3 GetLimbDarkeningFactor(float distSquared)
{
    switch(LimbDarkeningAlgorithm)
    {
        case LIMB_DARKENING_NONE:   return vec3(1.0);
        case LIMB_DARKENING_NEC96:  return GetNEC96LimbDarkeningFactor(distSquared);
        case LIMB_DARKENING_HM98:   return GetHM98LimbDarkeningFactor(distSquared);
        default:                    return vec3(1.0, 0.0, 1.0);
    }
}

void main()
{
    float distSquared = dot(TexCoord, TexCoord);
    float edgeWidth = length(vec2(dFdx(distSquared), dFdy(distSquared)));
    float alpha = 1.0 - smoothstep(1.0 - edgeWidth, 1.0, distSquared);
    if (distSquared > 1.0) discard;
    
    vec3 e_CameraPos = w_CameraPos - w_EarthCenterPos;
    vec3 e_Pos = w_Pos - w_EarthCenterPos;
    vec3 e_ViewDir = normalize(e_Pos - e_CameraPos);
    vec3 e_SunDir = w_SunDir;
    vec3 e_MoonDir = w_MoonDir;
    vec3 transmittance;
    vec3 solarSkyInscatter = GetSolarSkyRadiance(e_CameraPos, e_ViewDir, 0.0, e_SunDir, transmittance);
    vec3 lunarSkyInscatter = GetLunarSkyRadiance(e_CameraPos, e_ViewDir, 0.0, e_MoonDir, transmittance);
    vec3 inscatter = solarSkyInscatter + lunarSkyInscatter;
    vec3 radiance = GetSunRadiance() * GetLimbDarkeningFactor(distSquared);

    vec3 result = radiance * transmittance + inscatter;
    Color = vec4(result, alpha);
}

// NOTE: Invented by me
// vec3 GetOtherLimbDarkeningFactor(float centerToEdge)
// {
//   vec3 u = vec3(0.9, 0.9, 0.9);
//   float mu = centerToEdge;
//   vec3 factor = 1.0 - u * (1.0 - mu);
//   return factor;
// }