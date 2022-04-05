#version 330 core

in vec3 v_Normal;
in vec2 t_TexCoord;

uniform vec3 albedo;
uniform sampler2D albedoTexture;
uniform bool useAlbedoTexture;

out vec4 FragColor;

void main()
{
    vec3 N = normalize(v_Normal);
    vec3 color = albedo;
    if (useAlbedoTexture) color = texture(albedoTexture, t_TexCoord).rgb;
    FragColor = vec4(color * N.z, 1.0);
}