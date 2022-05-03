#pragma once

#include <string>
#include <string_view>

enum class ShaderType {VERTEX, FRAGMENT};

class ShaderSource
{
public:
    // NOTE: Might also add support for #inject
    ShaderSource(std::string_view path, std::string_view includesPath = "");
    void AddDefine(std::string symbol);
    std::string_view Get() const;
private:
    mutable std::string m_source; // NOTE: This is a bit of a hack required for the weird implementation of Get
};