
in vec3 v_LightDir;
in vec3 v_Normal;

out vec4 FragColor;

void main()
{
    vec3 L = normalize(v_LightDir);
    vec3 N = normalize(v_Normal);
    vec3 color = vec3(1.0, 0.0, 0.0);

    vec3 ambientStrength = vec3(0.3);
    vec3 ambientTerm = color * ambientStrength;

    vec3 diffuseStrength = vec3(0.7);
    float diffuseFactor = max(dot(N, L), 0.0);
    vec3 diffuseTerm = color * diffuseStrength * diffuseFactor;

    vec3 specularTerm = vec3(0.0);

    vec3 result = ambientTerm + diffuseTerm + specularTerm;

    FragColor = vec4(result, 1.0);
}