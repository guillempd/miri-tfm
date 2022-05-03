
// m_ : Model Space
// w_ : World Space
// v_ : View Space
// t_ : Tangent Space

layout (location = 0) in vec3 m_Pos;
layout (location = 1) in vec3 m_Normal;
layout (location = 2) in vec3 m_Tangent;
layout (location = 3) in vec3 m_Bitangent;
layout (location = 4) in vec2 _TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 w_LightDir;

// out vec3 t_Pos;
out vec3 t_LightDir;
// out vec3 t_CameraPos;
out vec2 TexCoord;

out vec3 w_Normal;
out vec3 w_Tangent;
out vec3 w_Bitangent;

void main()
{
    gl_Position = projection * view * model * vec4(m_Pos, 1.0);

    mat4 m_from_t = mat4(vec4(m_Tangent, 0.0), vec4(m_Bitangent, 0.0), vec4(m_Normal, 0.0), vec4(m_Pos, 1.0));
    mat4 t_from_m = inverse(m_from_t);

    mat4 w_from_m = model;
    mat4 m_from_w = inverse(w_from_m);

    mat4 t_from_w = t_from_m * m_from_w;

    // t_Pos = (t_from_m * vec4(m_Pos, 1.0)).xyz;
    t_LightDir = (t_from_w * vec4(w_LightDir, 0.0)).xyz;
    // t_CameraPos = (t_from_w * vec4(w_CameraPos, 1.0)).xyz;
    TexCoord = _TexCoord;

    w_Normal = inverse(transpose(mat3(model))) * m_Normal;
    w_Tangent = (model * vec4(m_Tangent, 0.0)).xyz;
    w_Bitangent = (model * vec4(m_Bitangent, 0.0)).xyz;
}