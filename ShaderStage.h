#pragma once

#include <glad/glad.h>

#include <string>

enum class ShaderType {VERTEX, FRAGMENT};

class ShaderStage
{
public:
    ShaderStage(ShaderType type);
    ~ShaderStage();
    void Compile(const std::string& path, const std::string& includesPath = "");
    GLuint m_id;
private:
    static std::string LoadSource(const std::string& path, const std::string& includesPath);
};