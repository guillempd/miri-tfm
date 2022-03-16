#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <string_view>

class Program
{
public:
    Program();
    void Build();
    void SetVertexShaderSource(std::string_view source) { m_vertexShaderSource = source; }
    void SetFragmentShaderSource(std::string_view source) { m_fragmentShaderSource = source; }
    void Use();
    void SetInt(std::string_view, int value);
    void SetFloat(std::string_view, float value);
    void SetVec3(std::string_view, const glm::vec3& value);
    void SetMat4(std::string_view, const glm::mat4& value);
    GLuint m_id;
private:
    std::string_view m_vertexShaderSource;
    std::string_view m_fragmentShaderSource;
};