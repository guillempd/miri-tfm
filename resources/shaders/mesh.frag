#version 330 core

// m_ : Model Space
// w_ : World Space
// v_ : View Space
// t_ : Tangent Space

in vec3 t_LightDir;
// in vec3 t_CameraPos;
in vec2 TexCoord;

uniform vec3 albedoConstant;
uniform sampler2D albedoTexture;
uniform bool useAlbedoTexture;
uniform sampler2D normalTexture;
uniform bool useNormalTexture;

out vec4 FragColor;

void main()
{
    vec3 L = t_LightDir;
    vec3 N = vec3(0.0, 0.0, 1.0);
    if (useNormalTexture) N = texture(normalTexture, TexCoord).xyz * 2.0 - 1.0;
    vec3 color = albedoConstant;
    if (useAlbedoTexture) color = texture(albedoTexture, TexCoord).rgb;

    vec3 ambientStrength = vec3(0.3);
    vec3 ambientTerm = color * ambientStrength;

    vec3 diffuseStrength = vec3(0.7);
    float diffuseFactor = max(dot(N, L), 0.0);
    vec3 diffuseTerm = color * diffuseStrength * diffuseFactor;

    vec3 specularTerm = vec3(0.0);

    vec3 result = ambientTerm + diffuseTerm + specularTerm;

    FragColor = vec4(result, 1.0);
}