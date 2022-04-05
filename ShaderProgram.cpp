#include "ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>

ShaderProgram::ShaderProgram()
    : m_id(GL_NONE)
{
    m_id = glCreateProgram();
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_id);
}

void ShaderProgram::AttachShader(GLuint id) const
{
    glAttachShader(m_id, id);
}

void ShaderProgram::Build()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSource = m_vertexShaderSource.data();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderSource = m_fragmentShaderSource.data();
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glAttachShader(m_id, vertexShader);
    glAttachShader(m_id, fragmentShader);
    glLinkProgram(m_id);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint success;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(m_id, 512, NULL, infoLog);
        std::cerr << infoLog << std::endl;
    }
}

void ShaderProgram::Use()
{
    glUseProgram(m_id);
}

void ShaderProgram::SetInt(std::string_view key, int value)
{
    glUniform1i(glGetUniformLocation(m_id, key.data()), value);
}

void ShaderProgram::SetFloat(std::string_view key, float value)
{
    glUniform1f(glGetUniformLocation(m_id, key.data()), value);
}

void ShaderProgram::SetVec3(std::string_view key, const glm::vec3& value)
{
    glUniform3fv(glGetUniformLocation(m_id, key.data()), 1, glm::value_ptr(value));
}

void ShaderProgram::SetMat4(std::string_view key, const glm::mat4& value)
{
    glUniformMatrix4fv(glGetUniformLocation(m_id, key.data()), 1, GL_FALSE, glm::value_ptr(value));
}
