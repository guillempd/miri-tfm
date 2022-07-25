#include "ShaderStage.h"

#define STB_INCLUDE_LINE_GLSL
#define STB_INCLUDE_IMPLEMENTATION
#include <stb_include.h>

#include <iostream>
#include <vector>


ShaderStage::ShaderStage()
    : m_id(GL_NONE)
{
}

ShaderStage::~ShaderStage()
{
    glDeleteShader(m_id);
}

void ShaderStage::Create(ShaderType type)
{
    if (m_id != GL_NONE) glDeleteShader(m_id);

    switch (type)
    {
    case ShaderType::VERTEX:
    {
        m_id = glCreateShader(GL_VERTEX_SHADER);
    } break;
    case ShaderType::FRAGMENT:
    {
        m_id = glCreateShader(GL_FRAGMENT_SHADER);
    } break;
    default:
    {
        std::cerr << "[ShaderStage] E: Unknown ShaderType " << static_cast<int>(type) << "." << std::endl;
    }
    }
}

void ShaderStage::Compile(const std::string& path, const std::string& includesPath)
{
    std::string source = LoadSource(path, includesPath);
    const char* sourceData = source.data();
    glShaderSource(m_id, 1, &sourceData, nullptr);
    glCompileShader(m_id);

    GLint success;
    glGetShaderiv(m_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLint length;
        glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &length);

        std::vector<GLchar> infoLog(length);
        glGetShaderInfoLog(m_id, length, &length, infoLog.data());

        std::cerr << "[ShaderStage] E: Compiling shader." << std::endl;
        std::cerr << infoLog.data() << std::endl;
    }
}

std::string ShaderStage::LoadSource(const std::string& path, const std::string& includesPath = "")
{
    char error[256];
    char* source = stb_include_file(const_cast<char*>(path.data()), "", const_cast<char*>(includesPath.data()), error);
    if (!source)
    {
        std::cerr << "[stb_include] E: " << error << std::endl;
        return "";
    }
    else
    {
        std::string result = std::string(source);
        free(source);
        return result;
    }
}
