#include "ShaderSource.h"

#define STB_INCLUDE_IMPLEMENTATION
#include <stb_include.h>

#include <iostream>

ShaderSource::ShaderSource(std::string_view path, std::string_view includesPath)
    : m_source(nullptr)
{
    char error[256];
    m_source = stb_include_file(const_cast<char*>(path.data()), "", const_cast<char*>(includesPath.data()), error);
    if (!m_source) std::cerr << "[stb_include] " << error << std::endl;
    // else std::cout << m_source << std::endl;
}

ShaderSource::~ShaderSource()
{
    free(m_source);
}

std::string_view ShaderSource::get() const
{
    return std::string_view(m_source);
}