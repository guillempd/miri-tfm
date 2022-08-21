#version 330 core

in vec2 TexCoord;

uniform sampler2D hdrTexture;
uniform float exposure;

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

vec3 tonemap(vec3 c)
{
    return c / (1.0 + c);
}

vec3 expose(vec3 c, float exposure)
{
    return exposure * c;
}

void main()
{
    vec3 hdrColor = texture(hdrTexture, TexCoord).rgb;
    hdrColor = expose(hdrColor, exposure);
    vec3 sdrColor = tonemap(hdrColor);
    vec3 srgbColor = toSrgb(sdrColor);
    FragColor.rgb = srgbColor;
    FragColor.a = 1.0;
}