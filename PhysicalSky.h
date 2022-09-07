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
    enum class SunLimbDarkeningAlgorithm {NONE, NEC96, HM98};
private:
    std::unique_ptr<atmosphere::Model> m_solarModel;
    std::unique_ptr<atmosphere::Model> m_lunarModel;
    AstronomicalPositioning m_astronomicalPositioning;

    // MODEL PARAMETERS PREFIX
    // d: Default
    // m: New
    // c: Current

    // GENERAL
    float m_dPlanetRadius;
    float m_nPlanetRadius;
    float m_cPlanetRadius;

    float m_dAtmosphereHeight;
    float m_nAtmosphereHeight;
    float m_cAtmosphereHeight;

    glm::vec3 m_dGroundAlbedo;
    glm::vec3 m_nGroundAlbedo;
    glm::vec3 m_cGroundAlbedo;

    // SUN
    ShaderProgram m_sunShader;

    float m_dSunSizeMultiplier;
    float m_nSunSizeMultiplier;
    float m_cSunSizeMultiplier;

    float m_dSunIrradiance;
    float m_nSunIrradiance;
    float m_cSunIrradiance;

    SunLimbDarkeningAlgorithm m_dSunLimbDarkeningAlgorithm;
    SunLimbDarkeningAlgorithm m_cSunLimbDarkeningAlgorithm;

    // MOON
    ShaderProgram m_moonShader;

    float m_dMoonSizeMultiplier;
    float m_nMoonSizeMultiplier;
    float m_cMoonSizeMultiplier;

    bool m_cMoonEarthshineEnable;
    bool m_dMoonEarthshineEnable;

    Texture m_moonColorMap;
    bool m_dMoonColorMapEnable;
    bool m_cMoonColorMapEnable;

    Texture m_moonNormalMap;
    float m_dMoonNormalMapStrength;
    float m_cMoonNormalMapStrength;

    // SKY
    ShaderProgram m_skyShader;
    Texture m_starsMap;
    float m_starsMapIntensity;

    // ARTIFICIAL LIGHT
    ShaderProgram m_lightShader;
    Mesh m_bulbMesh;

    bool m_cArtificialLightEnable;
    bool m_dArtificialLightEnable;

    glm::vec3 m_cArtificialLightPos;
    glm::vec3 m_dArtificialLightPos;

    float m_cArtificialLightRadiantIntensity;
    float m_dArtificialLightRadiantIntensity;

    // RAYLEIGH
    float m_dRayleighScatteringScale;
    float m_nRayleighScatteringScale;
    float m_cRayleighScatteringScale;

    glm::vec3 m_dRayleighScatteringCoefficient;
    glm::vec3 m_nRayleighScatteringCoefficient;
    glm::vec3 m_cRayleighScatteringCoefficient;

    float m_dRayleighExponentialDistribution;
    float m_nRayleighExponentialDistribution;
    float m_cRayleighExponentialDistribution;

    // MIE
    float m_dMieScatteringScale;
    float m_nMieScatteringScale;
    float m_cMieScatteringScale;

    glm::vec3 m_dMieScatteringCoefficient;
    glm::vec3 m_nMieScatteringCoefficient;
    glm::vec3 m_cMieScatteringCoefficient;

    float m_dMieAbsorptionScale;
    float m_nMieAbsorptionScale;
    float m_cMieAbsorptionScale;

    glm::vec3 m_dMieAbsorptionCoefficient;
    glm::vec3 m_nMieAbsorptionCoefficient;
    glm::vec3 m_cMieAbsorptionCoefficient;

    float m_dMieExponentialDistribution;
    float m_nMieExponentialDistribution;
    float m_cMieExponentialDistribution;

    float m_dMiePhaseFunctionG;
    float m_nMiePhaseFunctionG;
    float m_cMiePhaseFunctionG;

    // OZONE
    float m_dOzoneAbsorptionScale;
    float m_nOzoneAbsorptionScale;
    float m_cOzoneAbsorptionScale;

    glm::vec3 m_dOzoneAbsorptionCoefficient;
    glm::vec3 m_nOzoneAbsorptionCoefficient;
    glm::vec3 m_cOzoneAbsorptionCoefficient;

    // OTHERS
    bool m_notAppliedChanges;
    Mesh m_mesh;
    Mesh m_groundMesh;
    Mesh m_fullScreenQuadMesh;
    ShaderProgram m_meshShader;
};