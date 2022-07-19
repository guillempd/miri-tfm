#include "ShaderStage.h"

#define STB_INCLUDE_IMPLEMENTATION
#include <stb_include.h>

#include <iostream>

ShaderStage::ShaderStage(std::string_view path, std::string_view includesPath)
    : m_source()
{
    char error[256];
    char* source = stb_include_file(const_cast<char*>(path.data()), "", const_cast<char*>(includesPath.data()), error);
    if (!source) std::cerr << "[stb_include] " << error << std::endl;
    else
    {
        m_source = std::string(source);
        free(source);
    }
}

void ShaderStage::AddDefine(std::string symbol)
{
    m_source = "#define " + symbol + "\n" + m_source;
}

std::string_view ShaderStage::Get() const
{
    m_source = "#version 330 core\n" + m_source;
    return std::string_view(m_source);
}