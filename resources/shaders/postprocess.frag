#version 330 core

in vec2 TexCoord;

uniform sampler2D hdrTexture;
uniform float exposure;

uniform float max_white;

out vec4 FragColor;

// See: https://en.wikipedia.org/wiki/SRGB#Transformation
float toSrgb(float c)
{
    if (c <= 0.0031308) return 12.92 * c;
    else return 1.055 * pow(c, 1.0 / 2.4) - 0.055;
}

vec3 toSrgb(vec3 c)
{
    return vec3(toSrgb(c.x), toSrgb(c.y), toSrgb(c.z));
}

// vec3 tonemap(vec3 c, float exposure)
// {
//     return vec3(1.0) - exp(-exposure * c);
// }

vec3 expose(vec3 c, float exposure)
{
    return exposure * c;
}


vec3 reinhard(vec3 c)
{
    return c / (1.0 + c);
}

float reinhard(float l)
{
    return l / (1.0 + l);
}

float luminance(vec3 c)
{
    return dot(c, vec3(0.2126f, 0.7152f, 0.0722f));
}

vec3 reinhard_luminance(vec3 c)
{
    float l_in = luminance(c);
    float l_out = reinhard(l_in);
    return c * (l_out / l_in);
}

vec3 reinhard_extended(vec3 c, float max_white)
{
    return (c * (1.0 + c / vec3(max_white * max_white))) / (1.0 + c);
}

float reinhard_extended(float l, float max_white)
{
    return (l * (1.0 + l / (max_white * max_white))) / (1.0 + l);
}

vec3 reinhard_extended_luminance(vec3 c, float max_white)
{
    float l_in = luminance(c);
    float l_out = reinhard_extended(l_in, max_white);
    return c * (l_out / l_in);
}

void main()
{
    vec3 hdrColor = texture(hdrTexture, TexCoord).rgb;
    hdrColor = expose(hdrColor, exposure);

    vec3 sdrColor = reinhard_extended_luminance(hdrColor, max_white);
    vec3 srgbColor = toSrgb(sdrColor);

    FragColor.rgb = srgbColor;
    FragColor.a = 1.0;
}