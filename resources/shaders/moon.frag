#version 330 core

// m_ : Model Space
// w_ : World Space
// v_ : View Space
// t_ : Tangent Space
// p_ : Planet Space (World space shifted so that planet is at origin) // Earth space
// l_ : Lunar Space (Equivalent to model space)

#include "atmosphere.glsl"

const float PI = 3.14159265358979;
const vec3 SunIrradiance = vec3(1500.0) / 3.0; // Make uniform
const vec3 EarthIrradiance = vec3(0.05) / 3.0; // Make uniform
const vec3 Albedo = 0.072 * vec3(1.2525, 1.04125, 0.8625);

in vec2 TexCoord;

uniform mat4 Model;
uniform vec3 w_SunDir;
uniform vec3 w_MoonDir;
uniform vec3 w_EarthDir;
uniform vec3 w_CameraPos;
uniform vec3 w_PlanetPos;
uniform float EarthshineIntensity;
uniform sampler2D NormalMap;

out vec4 FragColor;

// Perform lighting in world space for the moment

vec3 GetN()
{
    vec3 l_Pos = vec3(TexCoord.st, sqrt(1.0 - TexCoord.s * TexCoord.s - TexCoord.t * TexCoord.t));
    // vec3 l_Normal = l_Pos;
    // vec3 w_Normal = (Model * vec4(l_Normal, 0.0)).xyz;

    float u = atan(l_Pos.x, l_Pos.z) / (2.0 * PI);
    float v = acos(l_Pos.y) / PI;

    vec3 N = l_Pos;
    vec3 B = normalize(vec3(-N.y * N.x, N.x * N.x + N.z * N.z, -N.y * N.z));
    vec3 T = cross(B, N);
    mat3 TBN = mat3(T, B, N);
    vec3 t_Normal = texture(NormalMap, vec2(u, v)).xyz * 2.0 - 1.0;
    vec3 m_Normal = TBN * t_Normal;
    vec3 w_Normal = inverse(transpose(mat3(Model))) * m_Normal;
    // return N;
    return normalize(w_Normal);
}

vec3 GetL()
{
    return normalize(w_SunDir);
}

vec3 GetV()
{
    vec3 l_Pos = vec3(TexCoord.st, sqrt(1.0 - TexCoord.s * TexCoord.s - TexCoord.t * TexCoord.t));
    vec3 w_Pos = (Model * vec4(l_Pos, 1.0)).xyz;
    return normalize(w_CameraPos - w_Pos);
}

float B(float phi)
{
    const float g = 0.6;
    if (phi < PI / 2.0)
    {
        float tanPhi = tan(phi);
        float expTerm = exp(-g / tanPhi);
        return 2.0 - tanPhi / (2 * g) * (1 - expTerm) * (3 - expTerm);
    }
    else return 1.0;
}

float S(float phi)
{
    const float t = 0.1;
    float sinPhi = sin(phi);
    float cosPhi = cos(phi);
    return (sinPhi + (PI - phi) * cosPhi) / PI + t * pow(1.0 - 0.5 * cosPhi, 2.0);
}

// TODO: Apply transmittance and inscatter for both sky models
void main()
{
    if (length(TexCoord) > 1.0) discard;

    vec3 N = GetN();
    vec3 V = GetV();
    vec3 L = normalize(w_SunDir);

    float phi = acos(dot(L, V));
    float cthetar = dot(N, V);
    float cthetai = max(dot(N, L), 0.0);

    vec3 radiance = vec3(0.0);

    float brdf = 2 / (3 * PI) * B(phi) * S(phi) / (1 + cthetar / cthetai);
    radiance += Albedo * brdf * SunIrradiance * cthetai;
    
    // Compute texture coordinates
    // vec3 l_Pos = vec3(TexCoord.st, sqrt(1.0 - TexCoord.s * TexCoord.s - TexCoord.t * TexCoord.t));
    // float u = atan(l_Pos.x, l_Pos.z) / (2.0 * PI);
    // float v = acos(l_Pos.y) / PI;
    // FragColor = vec4(N, 1.0);
    // vec3 albedo = texture(NormalMap, vec2(u, v)).rgb;



    // // vec3 Albedo = texture(NormalMap, vec2(u, v)).rgb; // TODO: Compute texture coordinates

    // EARTH LIGHT CONTRIBUTION
    // vec3 EarthIrradiance = vec3(EarthshineIntensity) / 3.0;
    // L = normalize(w_EarthDir);
    // phi = 0.0;
    // cthetar = dot(N, V);
    // cthetai = max(dot(N, L), 0.0);
    // brdf = 2 / (3 * PI) * B(phi) * S(phi) / (1 + cthetar / cthetai);
    // radiance += Albedo * brdf * EarthIrradiance * cthetar;

    // APPLY ATMOSPHERE
    vec3 p_SunDir = w_SunDir;
    vec3 p_MoonDir = w_MoonDir;
    vec3 p_CameraPos = w_CameraPos - w_PlanetPos;
    vec3 p_ViewDir = -V;

    vec3 transmittance;
    vec3 solarSkyInscatter = GetSolarSkyRadiance(p_CameraPos, p_ViewDir, 0.0, w_SunDir, transmittance);
    vec3 lunarSkyInscatter = GetLunarSkyRadiance(p_CameraPos, p_ViewDir, 0.0, w_MoonDir, transmittance);
    vec3 inscatter = solarSkyInscatter + lunarSkyInscatter;
    radiance = radiance * transmittance + inscatter;
    FragColor = vec4(radiance, 1.0);

    // vec3 radiance = vec3(100.0, 0.0, 100.0);
}