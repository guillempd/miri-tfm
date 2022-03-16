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
    std::vector<glm::uvec3> triangles;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        const aiVector3D& position = mesh->mVertices[i];
        positions.emplace_back(position.x, position.y, position.z);

        const aiVector3D& normal = mesh->mNormals[i];
        normals.emplace_back(normal.x, normal.y, normal.z);
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
    normals.emplace_back(0.0f, 10.0f, 1.0f);
    normals.emplace_back(10.0f, 0.0f, 1.0f);
    normals.emplace_back(1.0f, 1.0f, 1.0f);

    triangles.emplace_back(0, 1, 2);
    triangles.emplace_back(0, 2, 3);
    m_numElements = 3 * triangles.size();*/

    Upload(positions, normals, triangles);
}

void Mesh::Upload(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::uvec3>& triangles)
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, (positions.size() + normals.size()) * sizeof(glm::vec3), nullptr, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(glm::vec3), positions.data());
    glBufferSubData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), normals.size() * sizeof(glm::vec3), normals.data());

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * 3 * sizeof(unsigned int), triangles.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(positions.size() * sizeof(glm::vec3)));

    if (glGetError() != GL_NO_ERROR)
    {
        std::cerr << "[OpenGL] E: Uploading model" << std::endl;
    }
}

const char vertexShaderSource[] = R"(
    #version 330 core
    layout (location = 0) in vec3 m_Pos;
    layout (location = 1) in vec3 m_Normal;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    out vec3 v_Normal;
    void main()
    {
        gl_Position = projection * view * model * vec4(m_Pos, 1.0);
        v_Normal = inverse(transpose(mat3(view * model))) * m_Normal;
    }
)";
const char fragmentShaderSource[] = R"(
    #version 330 core
    in vec3 v_Normal;
    out vec4 FragColor;
    void main()
    {
        vec3 baseColor = vec3(0.7, 0.7, 0.7);
        vec3 N = normalize(v_Normal);
        FragColor = vec4(baseColor * N.z, 1.0);
    }
)";

void Mesh::Load()
{
    m_program.SetVertexShaderSource(vertexShaderSource);
    m_program.SetFragmentShaderSource(fragmentShaderSource);
    m_program.Build();
    Assimp::Importer importer = Assimp::Importer();
    const aiScene* scene = importer.ReadFile("D:/Descargas/horse_statue_01_4k.fbx/horse_statue_01_4k.fbx", aiProcess_Triangulate);
    if (scene) ProcessScene(scene);
}

void Mesh::Render(const Camera &camera)
{
    m_program.Use();
    glm::mat4 model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0));
    model = glm::scale(model, glm::vec3(3.0f));
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


Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteVertexArrays(1, &m_vao);
}
