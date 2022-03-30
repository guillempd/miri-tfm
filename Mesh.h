#pragma once

#include "Program.h"
#include "Camera.h"

#include <glad/glad.h>

#include <assimp/scene.h>

#include <glm/glm.hpp>

#include <vector>

class Mesh
{
public:
    Mesh() = default;
    ~Mesh();
    void Load();
    void Render(const Camera& camera);
    void JustRender(const Camera& camera);
private:
    void ProcessScene(const aiScene* scene);
    void ProcessMesh(aiMesh* mesh);
    void Upload(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::uvec3>& triangles);
private:
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
    unsigned int m_numElements;
    Program m_program;
};