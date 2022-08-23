#pragma once

#include "ShaderProgram.h"
#include "Camera.h"
#include "Texture.h"

#include <glad/glad.h>

#include <assimp/scene.h>

#include <glm/glm.hpp>

#include <vector>
#include <string_view>

class Mesh
{
public:
    Mesh();
    Mesh(std::string_view path);
    ~Mesh();
    void Render();
private:
    void ProcessScene(const aiScene* scene);
    void ProcessMesh(aiMesh* mesh);
    void Upload(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec3>& tangents, const std::vector<glm::vec3>& bitangents, const std::vector<glm::vec2>& texCoords, const std::vector<glm::uvec3>& triangles);
private:
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
    unsigned int m_numElements;
};