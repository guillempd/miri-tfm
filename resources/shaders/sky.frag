#version 330 core
#include "atmosphere.glsl"

// m_ : Model coordinate system
// w_ : World coordinate system
// v_ : View coordinate system
// t_ : Tangent coordinate system
// e_ : Earth coordinate system (Earth centric coordinate space, analogous to world space shifted so that the earth center is at the origin)

in vec3 w_ViewDir;

uniform vec3 w_CameraPos;
uniform vec3 w_EarthCenterPos;
uniform vec3 w_SunDir;
uniform vec3 w_MoonDir;

out vec4 Color;

void main()
{
    vec3 e_CameraPos = w_CameraPos - w_EarthCenterPos;
    vec3 e_ViewDir = normalize(w_ViewDir);
    vec3 e_SunDir = w_SunDir;
    vec3 e_MoonDir = w_MoonDir;
    vec3 transmittance;
    vec3 solarSkyInscatter = GetSolarSkyRadiance(e_CameraPos, e_ViewDir, 0.0, e_SunDir, transmittance);
    vec3 lunarSkyInscatter = GetLunarSkyRadiance(e_CameraPos, e_ViewDir, 0.0, e_MoonDir, transmittance);
    vec3 result = solarSkyInscatter + lunarSkyInscatter;
    Color = vec4(result, 1.0);
}