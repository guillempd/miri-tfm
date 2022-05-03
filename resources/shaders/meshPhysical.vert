layout (location = 0) in vec3 m_Pos;
layout (location = 1) in vec3 m_Normal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 w_Normal;
out vec3 w_Pos;
void main()
{
    mat3 normalMatrix = inverse(transpose(mat3(model)));
    w_Normal = normalMatrix * m_Normal;
    w_Pos = (model * vec4(m_Pos, 1.0)).xyz;
    gl_Position = projection * view * vec4(w_Pos, 1.0);
    // w_Normal = w_Normal.zxy;
    // w_Pos = w_Pos.zxy;
}