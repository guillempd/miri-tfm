#pragma once

#include "ShaderProgram.h"
#include "Camera.h"

#include <glm/glm.hpp>

#include <atmosphere/model.h>

class PhysicalSky
{
public:
    PhysicalSky();
    ~PhysicalSky();
    void Init();
    void InitResources();
    void InitShaders();
    void InitModel();
    void Update();
    void Render(const Camera& camera, const glm::vec2& sunAngles);
    void RenderDemo(const Camera& camera, const glm::vec2& sunAngles);
private:
    std::unique_ptr<atmosphere::Model> m_model;

    glm::vec3 m_groundAlbedo;
    float m_planetRadius;
    float m_atmosphereHeight;

    float m_sunIntensity;
    float m_sunAngularRadius;

    glm::vec3 m_rayleighScatteringCoefficient;
    float m_rayleighScatteringScale;
    float m_rayleighExponentialDistribution;

    glm::vec3 m_mieScatteringCoefficient;
    float m_mieScatteringScale;
    glm::vec3 m_mieAbsorptionCoefficient;
    float m_mieAbsorptionScale;
    float m_miePhaseFunctionG;
    float m_mieExponentialDistribution;

    glm::vec3 m_ozoneAbsorptionCoefficient;
    float m_ozoneAbsorptionScale;

    bool m_shouldRecomputeModel;

    int m_limbDarkeningStrategy;


    ShaderProgram m_skyShader;
    ShaderProgram m_demoShader;

    // Full screen quad resources
    // TODO: Make these a proper mesh object
    GLuint m_fullScreenQuadVao;
    GLuint m_fullScreenQuadVbo;

    // TODO: Remove these parameters
    bool use_combined_textures_;
    bool use_half_precision_;
};