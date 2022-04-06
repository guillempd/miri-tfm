#version 330 core

in vec3 w_Normal;
in vec3 w_Tangent;
in vec3 w_Bitangent;
in vec2 t_TexCoord;

uniform vec3 albedo;
uniform sampler2D albedoTexture;
uniform bool useAlbedoTexture;
uniform sampler2D normalTexture;
uniform bool useNormalTexture;

out vec4 FragColor;

void main()
{
    vec3 N = normalize(w_Normal);
    vec3 color = albedo;
    if (useAlbedoTexture) color = texture(albedoTexture, t_TexCoord).rgb;
    if (useNormalTexture) color = texture(normalTexture, t_TexCoord).rgb;
    color = w_Bitangent;
    FragColor = vec4(color, 1.0);
}