#version 330 core
#include "atmosphere.glsl"

uniform vec3 w_CameraPos;
uniform vec3 w_EarthCenterPos;
uniform vec3 w_SunDir;
uniform vec3 w_MoonDir;
const vec3 Albedo = vec3(0.18);

uniform vec3 w_LightPos;
uniform vec3 LightRadiantIntensity;

in vec3 w_Pos;
in vec3 w_Normal;

out vec4 Color;

void main()
{
    Color = vec4(0.0, 1.0, 0.0, 1.0);
    vec3 e_CameraPos = w_CameraPos - w_EarthCenterPos;
    vec3 e_Pos = w_Pos - w_EarthCenterPos;
    vec3 e_SunDir = w_SunDir;
    vec3 e_MoonDir = w_MoonDir;
    vec3 e_Normal = normalize(w_Normal);

    vec3 solarSkyIrradiance;
    vec3 sunIrradiance = GetSunAndSolarSkyIrradiance(e_Pos, e_Normal, e_SunDir, solarSkyIrradiance);
    vec3 lunarSkyIrradiance;
    vec3 moonIrradiance = GetMoonAndLunarSkyIrradiance(e_Pos, e_Normal, e_MoonDir, lunarSkyIrradiance);
    vec3 directIrradiance = sunIrradiance + moonIrradiance;
    vec3 indirectIrradiance = solarSkyIrradiance + lunarSkyIrradiance;
    
    vec3 transmittance;
    vec3 solarSkyInscatter = GetSolarSkyRadianceToPoint(e_CameraPos, e_Pos, 0.0, e_SunDir, transmittance);
    vec3 lunarSkyInscatter = GetLunarSkyRadianceToPoint(e_CameraPos, e_Pos, 0.0, e_MoonDir, transmittance);
    vec3 inscatter = solarSkyInscatter + lunarSkyInscatter;

    vec3 w_LightDir = w_LightPos - w_Pos;
    float dSquared = dot(w_LightDir, w_LightDir);
    w_LightDir = w_LightDir / sqrt(dSquared);
    vec3 lightIrradiance = LightRadiantIntensity / dSquared * max(dot(w_Normal, w_LightDir), 0.0);

    vec3 radiance = Albedo / PI * (lightIrradiance + directIrradiance + indirectIrradiance);
    vec3 result =  radiance * transmittance + inscatter;
    Color = vec4(result, 1.0);
}