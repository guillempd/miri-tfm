#version 330 core
#include "psrdnoise2.glsl"

const mat3 XYZ_from_RGB = mat3(vec3(0.4124, 0.2126, 0.0193), vec3(0.3576, 0.7152, 0.1192), vec3(0.1805, 0.0722, 0.9505));

in vec2 TexCoord;

uniform sampler2D hdrTexture;

uniform float Exposure;
uniform float max_white;

uniform float AspectRatio;
uniform float Time;

uniform vec3 BlueTint;
uniform float NoiseScale;
uniform float NoiseStrength;
uniform float NoiseSpeed;
uniform vec3 MesopicRange;

uniform int Mode;
#define MODE_DAY 0
#define MODE_NIGHT 1
#define MODE_PHOTOPIC_LUMINANCE 2
#define MODE_SCOTOPIC_LUMINANCE 3

out vec4 FragColor;

float luminance(vec3 c)
{
    const vec3 Y_from_RGB = vec3(XYZ_from_RGB[0][1], XYZ_from_RGB[1][1], XYZ_from_RGB[2][1]);
    return dot(c, Y_from_RGB);
}

// TODO: Improve notation / make it consistent with the writeup
vec3 reinhard(vec3 c)
{
    float L = luminance(c);
    float targetL = (L * (1.0 + L / (max_white * max_white))) / (1.0 + L);
    return c / L * targetL;
}

vec3 srgb_from_linear(vec3 linear)
{
    vec3 srgbLo = 12.92 * linear;
    vec3 srgbHi = 1.055 * pow(linear, vec3(1.0 / 2.4)) - 0.055;
    bvec3 lo = lessThanEqual(linear, vec3(0.0031308));
    vec3 srgb = mix(srgbHi, srgbLo, lo);
    return srgb;
}

float photopic_luminance_from_XYZ(vec3 XYZ)
{
    return XYZ.y;
}

float scotopic_luminance_from_XYZ(vec3 XYZ)
{
    return XYZ.y * (1.33 * (1.0 + (XYZ.y + XYZ.z)/ XYZ.x) - 1.68);
}

vec3 XYZ_from_linear(vec3 linear)
{
    return XYZ_from_RGB * linear;
}

float compute_noise(vec2 texCoord)
{
    texCoord *= NoiseScale;
    const mat2 m = mat2(1.6,  1.2, -1.2,  1.6); // See: https://www.shadertoy.com/view/lsf3WH
    vec2 ignore;
    float result = 0.0;
    result  = 0.5000 * psrdnoise(texCoord, vec2(0.0, 0.0), Time * NoiseSpeed, ignore); texCoord = m * texCoord;
    result += 0.2500 * psrdnoise(texCoord, vec2(0.0, 0.0), Time * NoiseSpeed, ignore); texCoord = m * texCoord;
    result += 0.1250 * psrdnoise(texCoord, vec2(0.0, 0.0), Time * NoiseSpeed, ignore); texCoord = m * texCoord;
    result += 0.0625 * psrdnoise(texCoord, vec2(0.0, 0.0), Time * NoiseSpeed, ignore);
    return result * NoiseStrength;
}

vec3 mode_selection(vec3 sdrColor)
{
    vec3 xyzColor = XYZ_from_linear(sdrColor);

    float Y = photopic_luminance_from_XYZ(xyzColor);
    float V = scotopic_luminance_from_XYZ(xyzColor);
    float noise = compute_noise(TexCoord * vec2(AspectRatio, 1.0));

    vec3 dayColor = sdrColor;
    vec3 nightColor = (V + noise) * BlueTint;

    float l = MesopicRange.x;
    float r = MesopicRange.y;
    float x = clamp(Y, l, r);
    float s = 1.0 - (x - l) / (r - l);
    // float s = 1.0 - smoothstep(l, r, Y); // Smoothstep version from Jonas Thesis
    vec3 mixColor = mix(dayColor, nightColor, s);

    switch (Mode)
    {
        case MODE_DAY:                  return dayColor;
        case MODE_NIGHT:                return mixColor;
        case MODE_PHOTOPIC_LUMINANCE:   return vec3(Y);
        case MODE_SCOTOPIC_LUMINANCE:   return vec3(V);
    }
}

void main()
{
    vec3 hdrColor = texture(hdrTexture, TexCoord).rgb;
    vec3 exposedHdrColor = hdrColor * Exposure;
    vec3 sdrColor = reinhard(exposedHdrColor);
    vec3 outColor = mode_selection(sdrColor);
    vec3 srgbColor = srgb_from_linear(outColor);
    FragColor = vec4(srgbColor, 1.0);
}