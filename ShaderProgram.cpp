#include "ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

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

static void CheckShaderCompileStatus(GLuint shaderId)
{
    GLint success;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLint infoLogLength;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<GLchar> infoLog = std::vector<GLchar>(infoLogLength);
        glGetShaderInfoLog(shaderId, infoLogLength, nullptr, infoLog.data());
        std::cerr << infoLog.data() << std::endl;
    }
}

static void CheckProgramLinkStatus(GLuint programId)
{
    // FIXME: Why is this never reporting linking errors?
    GLint success;
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLint infoLogLength;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<GLchar> infoLog = std::vector<GLchar>(infoLogLength);
        glGetShaderInfoLog(programId, infoLogLength, nullptr, infoLog.data());
        std::cerr << infoLog.data() << std::endl;
    }
}

void ShaderProgram::Build(std::string_view vertexSource, std::string_view fragmentSource)
{
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexSourceData = vertexSource.data();
    glShaderSource(vertexShaderId, 1, &vertexSourceData, nullptr);
    glCompileShader(vertexShaderId);
    CheckShaderCompileStatus(vertexShaderId);

    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentSourceData = fragmentSource.data();
    glShaderSource(fragmentShaderId, 1, &fragmentSourceData, nullptr);
    glCompileShader(fragmentShaderId);
    CheckShaderCompileStatus(fragmentShaderId);

    glAttachShader(m_id, vertexShaderId);
    glAttachShader(m_id, fragmentShaderId);
    glLinkProgram(m_id);
    CheckProgramLinkStatus(m_id);

    // TODO: glDetachShader()
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
}

void ShaderProgram::Build(const ShaderSource& vertexSource, const ShaderSource& fragmentSource)
{
    Build(vertexSource.get(), fragmentSource.get());
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
