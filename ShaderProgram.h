#pragma once

#include "ShaderSource.h"

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <string_view>

class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();
    void AttachShader(GLuint id) const;
    void Build(const ShaderSource& vertexSource, const ShaderSource& fragmentSource);
    void Use();
    void SetInt(std::string_view, int value);
    void SetFloat(std::string_view, float value);
    void SetBool(std::string_view, bool value);
    void SetVec3(std::string_view, const glm::vec3& value);
    void SetMat4(std::string_view, const glm::mat4& value);
    GLuint m_id;
private:
    void Build(std::string_view vertexSource, std::string_view fragmentSource);
};