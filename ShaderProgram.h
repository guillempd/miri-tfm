#pragma once

#include "ShaderStage.h"
#include "Texture.h"

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <string_view>
#include <vector>

class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();
    void Create();
    void AttachShader(GLuint id);
    void Build();
    void Use();
    void SetInt(std::string_view, int value);
    void SetFloat(std::string_view, float value);
    void SetBool(std::string_view, bool value);
    void SetVec3(std::string_view, const glm::vec3& value);
    void SetMat4(std::string_view, const glm::mat4& value);
    void SetTexture(std::string_view, unsigned int unit, const Texture& value);
    GLuint m_id;
private:
    std::vector<GLuint> m_attachedShaders;
};