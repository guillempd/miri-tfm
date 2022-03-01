#include "HdrSky.h"

void HdrSky::Load()
{
    const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    out vec2 texCoord;
    void main()
    {
        gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
        texCoord = 0.5 * (aPos.xy + vec2(1.0));
    }
)";

    const char* fragmentShaderSource = R"(
    #version 330 core
    in vec2 texCoord;
    out vec4 FragColor;
    uniform sampler2D hdrImage;
    uniform float verticalFov;
    uniform vec3 cameraRight;
    uniform vec3 cameraUp;
    uniform vec3 cameraForward;
    // TODO: Check this code extracted from learnopengl.com
    const vec2 invAtan = vec2(0.1591, 0.3183);
    vec2 SampleSphericalMap(vec3 v)
    {
        vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
        uv *= invAtan;
        uv += 0.5;
        return uv;
    }
    void main()
    {
        // NOTE: Be careful with all z coordinates (might be negated)
        // These would be uniforms
        // const vec3 cameraRight = vec3(1.0, 0.0, 0.0);
        // const vec3 cameraUp = vec3(0.0, 1.0, 0.0);
        // const vec3 cameraForward = vec3(0.0, 0.0, 1.0);
        const vec3 cameraPosition = vec3(0.0);
        // float verticalFov = 3.1415/4;
        float ar = float(4)/float(3);
        float near = 0.1;

        mat3 cameraRotation = mat3(cameraRight, cameraUp, cameraForward);
        vec2 ndc = 2 * texCoord - vec2(1.0);
        float top = near * tan(verticalFov/2);
        float right = top * ar;
        vec2 viewOffset = ndc * vec2(top, right);
        vec3 ray = cameraPosition + cameraRotation * vec3(viewOffset, -near); // FIXME: cameraPosition should not be used here
        vec3 direction = normalize(ray);
        // Sample with this direction the hdrImage

        FragColor = texture(hdrImage, SampleSphericalMap(direction));
    }
)";

    m_program.SetVertexShaderSource(vertexShaderSource);
    m_program.SetFragmentShaderSource(fragmentShaderSource);
    m_program.Build();

    m_texture.Load();

    LoadFullScreenQuad();
}

void HdrSky::LoadFullScreenQuad()
{
    float quadVertices[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f,

        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f
    };

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW); // TODO: Is this with &quadVertices (?)

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0); // TODO: Be careful with the offset
}

void HdrSky::Render(const Camera& camera)
{
    m_texture.SetUnit(0);
    m_program.Use();
    m_program.SetInt("hdrImage", 0);
    m_program.SetFloat("verticalFov", camera.GetVerticalFov());
    m_program.SetVec3("cameraRight", camera.GetRight());
    m_program.SetVec3("cameraUp", camera.GetUp());
    m_program.SetVec3("cameraForward", camera.GetForward());

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
