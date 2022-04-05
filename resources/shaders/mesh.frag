#version 330 core
in vec3 v_Normal;
out vec4 FragColor;

void main()
{
    vec3 baseColor = vec3(0.7, 0.7, 0.7);
    vec3 N = normalize(v_Normal);
    FragColor = vec4(baseColor * N.z, 1.0);
}