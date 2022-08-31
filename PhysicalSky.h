#pragma once

#include "ShaderProgram.h"
#include "Camera.h"
#include "Coordinates.h"
#include "Texture.h"
#include "Mesh.h"

#include <glm/glm.hpp>

#include <atmosphere/model.h>

class PhysicalSky
{
public:
    PhysicalSky();
    ~PhysicalSky();
    void Init();
    void InitNewParameters();
    void InitCurrentParameters();
    void InitResources();
    void InitShaders();
    void InitModel();
    void Update();
    void Render(const Camera& camera);
private:
    static glm::mat4 BillboardModelFromCamera(const glm::vec3& cameraPosition, const glm::vec3& billboardDirection);
    void RenderSun(const Camera& camera, const glm::vec3& sunWorldDirection, const glm::vec3& moonWorldDirection);
    void RenderMoon(const Camera& camera, const glm::vec3& sunWorldDirection, const glm::vec3& moonWorldDirection);
    void RenderSky(const Camera& camera, const glm::vec3& sunWorldDirection, const glm::vec3& moonWorldDirection);
    void RenderScene(const Camera& camera, const glm::vec3& sunWorldDirection, const glm::vec3& moonWorldDirection);
    static double VisibleLitFractionFromPhaseAngle(double phi);
private:
    enum class LimbDarkeningAlgorithm {NONE, NEC96, HM98};
private:
    std::unique_ptr<atmosphere::Model> m_solarModel;
    std::unique_ptr<atmosphere::Model> m_lunarModel;

    // MODEL PARAMETERS
    // d: Default
    // m: New
    // c: Current

    // General
    glm::vec3 m_dGroundAlbedo;
    glm::vec3 m_nGroundAlbedo;
    glm::vec3 m_cGroundAlbedo;

    float m_dPlanetRadius;
    float m_nPlanetRadius;
    float m_cPlanetRadius;

    float m_dAtmosphereHeight;
    float m_nAtmosphereHeight;
    float m_cAtmosphereHeight;

    // Sun
    float m_dSunIntensity;
    float m_nSunIntensity;
    float m_cSunIntensity;

    float m_dSunAngularRadius;
    float m_nSunAngularRadius;
    float m_cSunAngularRadius;

    LimbDarkeningAlgorithm m_dLimbDarkeningAlgorithm;
    LimbDarkeningAlgorithm m_nLimbDarkeningAlgorithm;
    LimbDarkeningAlgorithm m_cLimbDarkeningAlgorithm;

    // Moon
    float m_dMoonAngularRadius;
    float m_nMoonAngularRadius;
    float m_cMoonAngularRadius;

    // Sky
    float m_starsMapIntensity;

    // Rayleigh
    glm::vec3 m_dRayleighScatteringCoefficient;
    glm::vec3 m_nRayleighScatteringCoefficient;
    glm::vec3 m_cRayleighScatteringCoefficient;

    float m_dRayleighScatteringScale;
    float m_nRayleighScatteringScale;
    float m_cRayleighScatteringScale;

    float m_dRayleighExponentialDistribution;
    float m_nRayleighExponentialDistribution;
    float m_cRayleighExponentialDistribution;

    // Mie
    glm::vec3 m_dMieScatteringCoefficient;
    glm::vec3 m_nMieScatteringCoefficient;
    glm::vec3 m_cMieScatteringCoefficient;

    float m_dMieScatteringScale;
    float m_nMieScatteringScale;
    float m_cMieScatteringScale;

    glm::vec3 m_dMieAbsorptionCoefficient;
    glm::vec3 m_nMieAbsorptionCoefficient;
    glm::vec3 m_cMieAbsorptionCoefficient;

    float m_dMieAbsorptionScale;
    float m_nMieAbsorptionScale;
    float m_cMieAbsorptionScale;

    float m_dMiePhaseFunctionG;
    float m_nMiePhaseFunctionG;
    float m_cMiePhaseFunctionG;

    float m_dMieExponentialDistribution;
    float m_nMieExponentialDistribution;
    float m_cMieExponentialDistribution;

    // Ozone
    glm::vec3 m_dOzoneAbsorptionCoefficient;
    glm::vec3 m_nOzoneAbsorptionCoefficient;
    glm::vec3 m_cOzoneAbsorptionCoefficient;

    float m_dOzoneAbsorptionScale;
    float m_nOzoneAbsorptionScale;
    float m_cOzoneAbsorptionScale;

    bool m_shouldRecomputeModel;

    // SHADING RESOURCES
    ShaderProgram m_skyShader;
    ShaderProgram m_sunShader;
    ShaderProgram m_moonShader;
    ShaderProgram m_meshShader;
    Texture m_moonNormalMap;
    Texture m_starsMap;
    Mesh m_mesh;

    // Full screen quad resources
    // TODO: Make these a proper mesh object
    GLuint m_fullScreenQuadVao;
    GLuint m_fullScreenQuadVbo;

    // TODO: Remove these parameters
    bool use_combined_textures_;

    Coordinates m_coordinates;

};