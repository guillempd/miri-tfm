// m_ : Model Space
// w_ : World Space
// v_ : View Space
// t_ : Tangent Space

layout (location = 0) in vec3 m_Pos;
layout (location = 1) in vec3 m_Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 w_Pos;
out vec3 w_Normal;

void main()
{
    w_Pos = (model * vec4(m_Pos, 1.0)).xyz;
    mat3 normalMatrix = inverse(transpose(mat3(model)));
    w_Normal = normalMatrix * m_Normal;
    gl_Position = projection * view * vec4(w_Pos, 1.0);
}