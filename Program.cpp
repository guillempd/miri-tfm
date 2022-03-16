#include "Program.h"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>

Program::Program()
    : m_id(GL_NONE)
{
}

void Program::Build()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSource = m_vertexShaderSource.data();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderSource = m_fragmentShaderSource.data();
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    m_id = glCreateProgram();
    glAttachShader(m_id, vertexShader);
    glAttachShader(m_id, fragmentShader);
    glLinkProgram(m_id);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint success;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[256];
        glGetProgramInfoLog(m_id, 512, NULL, infoLog);
        std::cerr << infoLog << std::endl;
    }
}

void Program::Use()
{
    glUseProgram(m_id);
}

void Program::SetInt(std::string_view key, int value)
{
    glUniform1i(glGetUniformLocation(m_id, key.data()), value);
}

void Program::SetFloat(std::string_view key, float value)
{
    glUniform1f(glGetUniformLocation(m_id, key.data()), value);
}

void Program::SetVec3(std::string_view key, const glm::vec3& value)
{
    glUniform3fv(glGetUniformLocation(m_id, key.data()), 1, glm::value_ptr(value));
}

void Program::SetMat4(std::string_view key, const glm::mat4& value)
{
    glUniformMatrix4fv(glGetUniformLocation(m_id, key.data()), 1, GL_FALSE, glm::value_ptr(value));
}
