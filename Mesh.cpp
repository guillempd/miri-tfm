#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <glm/ext/matrix_transform.hpp>

#include <iostream>

void Mesh::ProcessScene(const aiScene* scene)
{
    if (scene->mNumMeshes == 0)
    {
        std::cerr << "[assimp] E: No meshes found in the imported model." << std::endl;
        return;
    }

    aiMesh* mesh = scene->mMeshes[0];
    ProcessMesh(mesh);
}

void Mesh::ProcessMesh(aiMesh* mesh)
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::uvec3> triangles;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        const aiVector3D& position = mesh->mVertices[i];
        positions.emplace_back(position.x, position.y, position.z);

        const aiVector3D& normal = mesh->mNormals[i];
        normals.emplace_back(normal.x, normal.y, normal.z);

        const aiVector3D& texCoord = mesh->mTextureCoords[0][i];
        texCoords.emplace_back(texCoord.x, texCoord.y);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace& face = mesh->mFaces[i];
        triangles.emplace_back(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
    }

    m_numElements = 3 * triangles.size();

    /*positions.emplace_back(-1.0f, 1.0f, 0.0f);
    positions.emplace_back(-1.0f, -1.0f, 0.0f);
    positions.emplace_back(1.0f, -1.0f, 0.0f);
    positions.emplace_back(1.0f, 1.0f, 0.0f);

    normals.emplace_back(0.0f, 0.0f, 1.0f);
    normals.emplace_back(0.0f, 0.0f, 1.0f);
    normals.emplace_back(0.0f, 0.0f, 1.0f);
    normals.emplace_back(0.0f, 0.0f, 1.0f);

    texCoords.emplace_back(0.0f, 1.0f);
    texCoords.emplace_back(0.0f, 0.0f);
    texCoords.emplace_back(1.0f, 0.0f);
    texCoords.emplace_back(1.0f, 1.0f);

    triangles.emplace_back(0, 1, 2);
    triangles.emplace_back(0, 2, 3);
    m_numElements = 3 * triangles.size();*/

    Upload(positions, normals, texCoords, triangles);
}

void Mesh::Upload(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<glm::uvec3>& triangles)
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, (positions.size() + normals.size() + texCoords.size()) * sizeof(glm::vec3), nullptr, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER,                                                       0, positions.size() * sizeof(glm::vec3), positions.data());
    glBufferSubData(GL_ARRAY_BUFFER,                    positions.size() * sizeof(glm::vec3),   normals.size() * sizeof(glm::vec3), normals.data());
    glBufferSubData(GL_ARRAY_BUFFER, (positions.size() + normals.size()) * sizeof(glm::vec3), texCoords.size() * sizeof(glm::vec2), texCoords.data());

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * 3 * sizeof(unsigned int), triangles.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(positions.size() * sizeof(glm::vec3)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)((positions.size() + normals.size()) * sizeof(glm::vec3)));

    if (glGetError() != GL_NO_ERROR)
    {
        std::cerr << "[OpenGL] E: Uploading model" << std::endl;
    }
}

void Mesh::Load()
{
    ShaderSource vertexShaderSource = ShaderSource("D:/dev/miri-tfm/resources/shaders/mesh.vert");
    ShaderSource fragmentShaderSource = ShaderSource("D:/dev/miri-tfm/resources/shaders/mesh.frag");
    m_program.Build(vertexShaderSource, fragmentShaderSource);
    Assimp::Importer importer = Assimp::Importer();
    const aiScene* scene = importer.ReadFile("D:/Descargas/horse_statue_01_4k.gltf/horse_statue_01_4k.gltf", aiProcess_Triangulate);
    if (scene) ProcessScene(scene);
}

void Mesh::Render(const Camera &camera)
{
    m_program.Use();
    glm::mat4 model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0));
    model = glm::scale(model, glm::vec3(1.0f));
    m_program.SetMat4("model", model);
    m_program.SetMat4("view", camera.GetViewMatrix());
    m_program.SetMat4("projection", camera.GetProjectionMatrix());
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_numElements, GL_UNSIGNED_INT, 0);

    if (glGetError() != GL_NO_ERROR)
    {
        std::cerr << "[OpenGL] E: Rendering mesh." << std::endl;
    }
}

void Mesh::JustRender(const Camera& camera)
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_numElements, GL_UNSIGNED_INT, 0);
}


Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteVertexArrays(1, &m_vao);
}
