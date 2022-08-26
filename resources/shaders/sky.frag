#version 330 core
#include "atmosphere.glsl"

// m_ : Model coordinate system
// w_ : World coordinate system
// v_ : View coordinate system
// t_ : Tangent coordinate system
// e_ : Earth coordinate system (Earth centric coordinate space, analogous to world space shifted so that the earth center is at the origin, this is the one that has to be used for atmospheric functions)

in vec3 w_ViewDir;

uniform vec3 w_CameraPos;
uniform vec3 w_EarthCenterPos;
uniform vec3 w_SunDir;
uniform vec3 w_MoonDir;

uniform sampler2D StarsMap;
uniform float StarsMapIntensity;

out vec4 Color;

void main()
{
    vec3 e_CameraPos = w_CameraPos - w_EarthCenterPos;
    vec3 e_ViewDir = normalize(w_ViewDir);
    vec3 e_SunDir = w_SunDir;
    vec3 e_MoonDir = w_MoonDir;

    float u = atan(e_ViewDir.x, e_ViewDir.z) / (2.0 * PI);
    float v = 1.0 - acos(e_ViewDir.y) / PI;
    vec3 radiance = texture(StarsMap, vec2(u, v)).rgb * StarsMapIntensity;

    vec3 transmittance;
    vec3 solarSkyInscatter = GetSolarSkyRadiance(e_CameraPos, e_ViewDir, 0.0, e_SunDir, transmittance);
    vec3 lunarSkyInscatter = GetLunarSkyRadiance(e_CameraPos, e_ViewDir, 0.0, e_MoonDir, transmittance);
    vec3 inscatter = solarSkyInscatter + lunarSkyInscatter;
    vec3 result = radiance + inscatter;

    Color = vec4(result, 1.0);
}