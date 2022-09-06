#pragma once

#include "ShaderProgram.h"
#include "Camera.h"
#include "AstronomicalPositioning.h"
#include "Texture.h"
#include "Mesh.h"

#include <glm/glm.hpp>

#include <atmosphere/model.h>

class PhysicalSky
{
public:
    PhysicalSky();
    ~PhysicalSky() = default;
    void Init();
    void MakeDefaultParametersNew();
    void MakeNewParametersCurrent();
    void InitResources();
    void InitShaders();
    void InitModel();
    void Update();
    void Render(const Camera& camera);
private:
    bool AnyChange();
    void ResetDefaults();
    glm::dvec3 ComputeMoonIrradiance();
    static glm::mat4 BillboardModelFromCamera(const glm::vec3& cameraPosition, const glm::vec3& billboardDirection);
    void RenderSun(const Camera& camera, const glm::vec3& sunWorldDirection, const glm::vec3& moonWorldDirection, float tanSunAngularRadius);
    void RenderMoon(const Camera& camera, const glm::vec3& sunWorldDirection, const glm::vec3& moonWorldDirection, float tanMoonAngularRadius);
    void RenderSky(const Camera& camera, const glm::vec3& sunWorldDirection, const glm::vec3& moonWorldDirection);
    void RenderScene(const Camera& camera, const glm::vec3& sunWorldDirection, const glm::vec3& moonWorldDirection);
    void RenderLight(const Camera& camera);
    static double VisibleLitFractionFromPhaseAngle(double phi);
    double ComputeEarthshineIrradiance();
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

    float m_dSunSizeMultiplier;
    float m_nSunSizeMultiplier;
    float m_cSunSizeMultiplier;

    LimbDarkeningAlgorithm m_dLimbDarkeningAlgorithm;
    LimbDarkeningAlgorithm m_cLimbDarkeningAlgorithm;

    // Moon
    float m_dMoonSizeMultiplier;
    float m_nMoonSizeMultiplier;
    float m_cMoonSizeMultiplier;

    float m_dMoonNormalMapStrength;
    float m_cMoonNormalMapStrength;

    bool m_dMoonUseColorMap;
    bool m_cMoonUseColorMap;

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

    bool m_notAppliedChanges;

    // SHADING RESOURCES
    ShaderProgram m_skyShader;
    ShaderProgram m_sunShader;
    ShaderProgram m_moonShader;
    ShaderProgram m_meshShader;
    Texture m_moonNormalMap;
    Texture m_moonColorMap;
    Texture m_starsMap;
    Mesh m_mesh;

    AstronomicalPositioning m_astronomicalPositioning;

    float m_cLightRadiantIntensity;
    float m_dLightRadiantIntensity;

    glm::vec3 m_LightPos;

    Mesh m_groundMesh;
    Mesh m_bulbMesh;
    bool m_cEnableLight;
    bool m_dEnableLight;
    ShaderProgram m_lightShader;
    Mesh m_fullScreenQuadMesh;
};