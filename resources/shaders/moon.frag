#version 330 core

// m_ : Model Space
// w_ : World Space
// v_ : View Space
// t_ : Tangent Space
// p_ : Planet Space (World space shifted so that planet is at origin)
// l_ : Lunar Space (Equivalent to model space)

const float PI = 3.14159265358979;
const vec3 SunIrradiance = vec3(500.0, 500.0, 500.0);
const vec3 EarthIrradiance = vec3(1.0, 1.0, 1.0);

in vec2 TexCoord;

uniform mat4 Model;
uniform vec3 w_SunDir;
uniform vec3 w_EarthDir;
uniform vec3 w_CameraPos;

out vec4 FragColor;

// Perform lighting in world space for the moment

vec3 GetN()
{
    vec3 l_Pos = vec3(TexCoord.st, sqrt(1.0 - TexCoord.s * TexCoord.s - TexCoord.t * TexCoord.t));
    vec3 l_Normal = l_Pos;
    vec3 w_Normal = (Model * vec4(l_Normal, 0.0)).xyz;
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

    vec3 radiance = vec3(0.0);
    // SUN LIGHT CONTRIBUTION
    vec3 L = normalize(w_SunDir);

    float phi = acos(dot(L, V));
    float cthetar = dot(N, V);
    float cthetai = max(dot(N, L), 0.0);

    float brdf = 2 / (3 * PI) * B(phi) * S(phi) / (1 + cthetar / cthetai);
    radiance += brdf * SunIrradiance * cthetai;

    // EARTH LIGHT CONTRIBUTION
    L = normalize(w_EarthDir);
    phi = 0.0;
    cthetar = dot(N, V);
    cthetai = max(dot(N, L), 0.0);
    brdf = 2 / (3 * PI) * B(phi) * S(phi) / (1 + cthetar / cthetai);
    radiance += brdf * EarthIrradiance * cthetar;

    FragColor = vec4(radiance, 1.0);
}