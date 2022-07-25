#include "ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

ShaderProgram::ShaderProgram()
    : m_id(GL_NONE)
    , m_attachedShaders()
{
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_id);
}

void ShaderProgram::Create()
{
    if (m_id != GL_NONE) glDeleteProgram(m_id);
    m_id = glCreateProgram();
}

void ShaderProgram::AttachShader(GLuint id)
{
    glAttachShader(m_id, id);
    m_attachedShaders.push_back(id);
}

void ShaderProgram::Build()
{
    glLinkProgram(m_id);

    GLint success;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLint length;
        glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &length);

        std::vector<GLchar> infoLog(length);
        glGetProgramInfoLog(m_id, length, &length, infoLog.data());

        std::cerr << "[ShaderProgram] E: Linking shader." << std::endl;
        std::cerr << infoLog.data() << std::endl;
    }

    for (GLuint attachedShader : m_attachedShaders) glDetachShader(m_id, attachedShader);
    m_attachedShaders.clear();
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

void ShaderProgram::SetBool(std::string_view key, bool value)
{
    glUniform1i(glGetUniformLocation(m_id, key.data()), value);
}

void ShaderProgram::SetVec3(std::string_view key, const glm::vec3& value)
{
    glUniform3fv(glGetUniformLocation(m_id, key.data()), 1, glm::value_ptr(value));
}

void ShaderProgram::SetMat4(std::string_view key, const glm::mat4& value)
{
    glUniformMatrix4fv(glGetUniformLocation(m_id, key.data()), 1, GL_FALSE, glm::value_ptr(value));
}
