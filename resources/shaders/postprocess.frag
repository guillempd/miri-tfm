#version 330 core

in vec2 TexCoord;

uniform sampler2D hdrTexture;
uniform float exposure;

out vec4 FragColor;

void main()
{
    // FragColor.rgb = vec3(0.666, 0.666, 0.666); // NOTE: Just to test this is working appropriately
    vec3 hdrColor = texture(hdrTexture, TexCoord).rgb;
    FragColor.rgb = 1.0 - exp(-exposure * hdrColor);
    FragColor.a = 1.0;
}