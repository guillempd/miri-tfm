#version 330 core

const mat3 XYZ_from_RGB = mat3(vec3(0.4124, 0.2126, 0.0193), vec3(0.3576, 0.7152, 0.1192), vec3(0.1805, 0.0722, 0.9505));

in vec2 TexCoord;

uniform sampler2D hdrTexture;
uniform float exposure;
uniform float max_white;

uniform vec3 blue_tint; // 1:1:2 ratio for this tint works well

uniform int mode;
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

// TODO: Apply noise
// TODO: Properly compute s
vec3 mode_selection(vec3 sdrColor)
{
    vec3 xyzColor = XYZ_from_linear(sdrColor);

    float Y = photopic_luminance_from_XYZ(xyzColor);
    float V = scotopic_luminance_from_XYZ(xyzColor);

    vec3 dayColor = sdrColor;
    vec3 nightColor = V * blue_tint;

    float s = 1.0 - smoothstep(0.0, 1.0, Y); // based on Y // Jonas Thesis
    vec3 mixColor = mix(dayColor, nightColor, s);

    switch (mode)
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
    vec3 exposedHdrColor = hdrColor * exposure;
    vec3 sdrColor = reinhard(exposedHdrColor);
    vec3 outColor = mode_selection(sdrColor);
    vec3 srgbColor = srgb_from_linear(outColor);
    FragColor = vec4(srgbColor, 1.0);
}