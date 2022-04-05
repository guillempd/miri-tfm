#pragma once

#include <string_view>

enum class ShaderType {VERTEX, FRAGMENT};

class ShaderSource
{
public:
    // NOTE: Might also add support for #inject
    ShaderSource(std::string_view path, std::string_view includesPath = "");
    ~ShaderSource();
    std::string_view get() const;
private:
    char* m_source;
};