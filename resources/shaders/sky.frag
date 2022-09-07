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
uniform float StarsMapMultiplier;

uniform sampler2D MilkywayMap;
uniform float MilkywayMapMultiplier;

uniform float T;
uniform float lon;
uniform float lat;

out vec4 Color;

mat3 Rx(float a)
{
    float sa = sin(a);
    float ca = cos(a);
    return mat3(vec3(1.0, 0.0, 0.0), vec3(0.0, ca, sa), vec3(0.0, -sa, ca));
}

mat3 Ry(float a)
{
    float sa = sin(a);
    float ca = cos(a);
    return mat3(vec3(ca, 0.0, -sa), vec3(0.0, 1.0, 0.0), vec3(sa, 0.0, ca));
}

mat3 Rz(float a)
{
    float sa = sin(a);
    float ca = cos(a);
    return mat3(vec3(ca, sa, 0.0), vec3(-sa, ca, 0.0), vec3(0.0, 0.0, 1.0));
}

vec3 RectangularHorizonToRectangularEquatorial(vec3 rectangularHorizon)
{
    float LMST = 4.894961 + 230121.675315 * T + lon;
    mat3 P = Rz(-0.01118*T) * Ry(0.00972*T) * Rz(-0.01118*T);
    mat3 M = P * Rz(LMST) * Ry(PI/2.0 - lat);
    vec3 rectangularEquatorial = M * rectangularHorizon;
    return rectangularEquatorial;
}

vec3 SphericalToRectangular(vec3 spherical)
{
    float lon = spherical.x;
    float lat = spherical.y;
    float r = spherical.z;
    vec3 rectangular;
    rectangular.x = cos(lat) * cos(lon);
    rectangular.y = cos(lat) * sin(lon);
    rectangular.z = sin(lat);
    return rectangular;
}

vec3 RectangularToSpherical(vec3 rectangular)
{
    float r = length(rectangular);
    float lon = atan(rectangular.y, rectangular.x);
    float lat = acos(rectangular.z / r);
    return vec3(lon, lat, r);
}

vec3 WorldToHorizon(vec3 world)
{
    return vec3(world.z, world.x, world.y);
}

void main()
{
    vec3 e_CameraPos = w_CameraPos - w_EarthCenterPos;
    vec3 e_ViewDir = normalize(w_ViewDir);
    vec3 e_SunDir = w_SunDir;
    vec3 e_MoonDir = w_MoonDir;

    vec3 rh_ViewDir = WorldToHorizon(e_ViewDir);
    vec3 re_ViewDir = RectangularHorizonToRectangularEquatorial(rh_ViewDir);
    vec3 se_ViewDir = RectangularToSpherical(re_ViewDir);
    vec2 uv = vec2(0.5, 1.0) - se_ViewDir.xy / vec2(2.0 * PI, PI);

    vec3 radiance = vec3(0.0);

    radiance += texture(StarsMap, uv).rgb * StarsMapMultiplier;
    radiance += texture(MilkywayMap, uv).rgb * MilkywayMapMultiplier;

    vec3 transmittance;
    vec3 solarSkyInscatter = GetSolarSkyRadiance(e_CameraPos, e_ViewDir, 0.0, e_SunDir, transmittance);
    vec3 lunarSkyInscatter = GetLunarSkyRadiance(e_CameraPos, e_ViewDir, 0.0, e_MoonDir, transmittance);
    vec3 inscatter = solarSkyInscatter + lunarSkyInscatter;
    vec3 result = radiance + inscatter;

    Color = vec4(result, 1.0);
}