#version 330 core

in vec2 TexCoord;

uniform mat4 model;
uniform vec3 w_SunDirection;

out vec4 FragColor;

// m_ : Model Space
// w_ : World Space
// v_ : View Space
// t_ : Tangent Space
// p_ : Planet Space (World space shifted so that planet is at origin)
// l_ : Lunar Space (Equivalent to model space)

void main()
{
    if (length(TexCoord) > 1.0) discard;

    vec3 l_Pos = vec3(TexCoord.st, sqrt(1.0 - TexCoord.s * TexCoord.s - TexCoord.t * TexCoord.t));
    vec3 l_Normal = l_Pos;
    vec3 w_Normal = (model * vec4(l_Normal, 0.0)).xyz;
    vec3 N = normalize(w_Normal);
    vec3 L = normalize(w_SunDirection);

    const vec3 albedo = vec3(0.3);

    const vec3 ambientIntensity = vec3(0.3);
    vec3 ambient = albedo * ambientIntensity;

    float diffuseIntensity = max(dot(N, L), 0.0);
    vec3 diffuse = albedo * diffuseIntensity;

    vec3 result = diffuse;

    // vec3 color = 1000.0 * vec3(TexCoord, 0.0);
    FragColor = vec4(10000.0 * result, 1.0);
}