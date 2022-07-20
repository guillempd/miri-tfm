#version 330 core

in vec2 TexCoord;

uniform sampler2D hdrTexture;

out vec4 FragColor;

void main()
{
    FragColor.rgb = vec3(0.0, 1.0, 0.0); // NOTE: Just to test this is working appropriately
    FragColor = texture(hdrTexture, TexCoord);
    FragColor.a = 1.0;
}