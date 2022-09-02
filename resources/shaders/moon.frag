#version 330 core
#include "atmosphere.glsl"

// m_ : Model Space
// w_ : World Space
// v_ : View Space
// t_ : Tangent Space
// p_ : Planet Space (World space shifted so that planet is at origin) // Earth space
// l_ : Lunar Space (Equivalent to model space)

// const vec3 SunIrradiance = vec3(1500.0) / 3.0; // Make uniform
// const vec3 EarthIrradiance = vec3(0.05) / 3.0; // Make uniform
const vec3 Albedo = 0.072 * vec3(1.2525, 1.04125, 0.8625);

in vec2 TexCoord;

uniform mat4 Model;
uniform vec3 w_SunDir;
uniform vec3 w_MoonDir;
uniform vec3 w_EarthDir;
uniform vec3 w_CameraPos;
uniform vec3 w_PlanetPos;
uniform float EarthIrradiance;
uniform float SunIrradiance;

uniform sampler2D ColorMap;
uniform sampler2D NormalMap;
uniform float NormalMapStrength;
uniform int UseColorMap;

out vec4 FragColor;

vec3 SampleNormalMap(vec3 m_Pos, vec2 texCoord)
{
    vec3 m_Normal = m_Pos;
    vec3 w_Normal = inverse(transpose(mat3(Model))) * m_Normal;

	vec3 N = w_Normal;
	vec3 B = normalize(vec3(-N.y * N.x, N.x * N.x + N.z * N.z, -N.y * N.z));
	vec3 T = cross(B, N);
	mat3 TBN = mat3(T, B, N);

	vec3 sampledNormal = texture(NormalMap, texCoord).xyz * 2.0 - 1.0;
	const vec3 defaultNormal = vec3(0.0, 0.0, 1.0);
	vec3 t_Normal = normalize(NormalMapStrength * sampledNormal + (1.0 - NormalMapStrength) * defaultNormal);

    return TBN * t_Normal;
}

float B(float phi)
{
    const float g = 0.8;
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

float brdf(float phi, float cthetar, float cthetai)
{
    return 2 / (3 * PI) * B(phi) * S(phi) / (1 + cthetar / cthetai);
}

// See https://svs.gsfc.nasa.gov/cgi-bin/details.cgi?aid=4720
vec3 SampleColorMap(vec2 texCoord)
{
	vec3 gammaColor = texture(ColorMap, texCoord).rgb;
	vec3 linearColor = pow(gammaColor, vec3(2.8));
	vec3 correctedLinearColor = linearColor / vec3(0.935, 1.005, 1.04);
	return correctedLinearColor;
}

vec3 RadianceContribution(vec3 N, vec3 V, vec3 L, vec3 irradiance)
{
    float phi = acos(dot(L, V));
    float cthetar = dot(N, V);
    float cthetai = max(dot(N, L), 0.0);
    return Albedo * brdf(phi, cthetar, cthetai) * irradiance * cthetai;
}

void main()
{
    float distSquared = dot(TexCoord, TexCoord);
    float edgeWidth = length(vec2(dFdx(distSquared), dFdy(distSquared)));
    float alpha = 1.0 - smoothstep(1.0 - edgeWidth, 1.0, distSquared);
    if (distSquared > 1.0) discard;

    vec3 m_Pos = vec3(TexCoord.x, TexCoord.y, sqrt(1.0 - TexCoord.x * TexCoord.x - TexCoord.y * TexCoord.y));
    vec3 w_Pos = (Model * vec4(m_Pos, 1.0)).xyz;

    float u = atan(m_Pos.x, m_Pos.z) / (2.0 * PI) + 0.5;
    float v = 1.0 - acos(m_Pos.y) / PI;
    vec2 texCoord = vec2(u, v);

    vec3 N = SampleNormalMap(m_Pos, texCoord);
    vec3 V = normalize(w_CameraPos - w_Pos);

    vec3 color = vec3(1.0);
    if (bool(UseColorMap)) color = SampleColorMap(texCoord);

    vec3 radiance = vec3(0.0);

    vec3 sunL = normalize(w_SunDir);
    vec3 sunIrradiance = vec3(1.0, 1.0, 1.0) * (SunIrradiance / 3.0);
    radiance += color * RadianceContribution(N, V, sunL, sunIrradiance);

    vec3 earthL = normalize(w_EarthDir);
    vec3 earthIrradiance = vec3(1.0, 1.0, 1.0) * (EarthIrradiance / 3.0);
    radiance += color * RadianceContribution(N, V, earthL, earthIrradiance);

    vec3 p_SunDir = w_SunDir;
    vec3 p_MoonDir = w_MoonDir;
    vec3 p_CameraPos = w_CameraPos - w_PlanetPos;
    vec3 p_ViewDir = -V;

    vec3 transmittance;
    vec3 solarSkyInscatter = GetSolarSkyRadiance(p_CameraPos, p_ViewDir, 0.0, w_SunDir, transmittance);
    vec3 lunarSkyInscatter = GetLunarSkyRadiance(p_CameraPos, p_ViewDir, 0.0, w_MoonDir, transmittance);
    vec3 inscatter = solarSkyInscatter + lunarSkyInscatter;
    vec3 result = radiance * transmittance + inscatter;
    FragColor = vec4(result, alpha);
}