#include "PhysicalSky.h"

#include <imgui.h>

#include <glm/gtc/type_ptr.hpp>

#include <iostream>

namespace {
    constexpr double kLengthUnitInMeters = 1000.0;
}  // anonymous namespace

using namespace atmosphere;

// TODO: Make use of doubles
PhysicalSky::PhysicalSky()
    : m_dPlanetRadius(6360.0f) // km
    , m_dAtmosphereHeight(60.0f) // km
    , m_dGroundAlbedo(0.401978f, 0.401978f, 0.401978f) // unitless

    , m_dSunSizeMultiplier(5.0f) // unitless
    , m_dSunIrradiance(1500.000000f) // W*m^-2
    , m_dSunLimbDarkeningAlgorithm(SunLimbDarkeningAlgorithm::NONE)

    , m_dMoonSizeMultiplier(5.0f) // unitless
    , m_dMoonEarthshineEnable(true)
    , m_dMoonColorMapEnable(true)
    , m_dMoonNormalMapStrength(0.75f)

    , m_dSkyStarsMapMultiplier(-1.0f)
    , m_dSkyMilkywayMapMultiplier(-1.0f)

    , m_dArtificialLightEnable(false)
    , m_dArtificialLightPos(0.0f, 5.0f, 0.0f)
    , m_dArtificialLightRadiantIntensity(1.0f) // W*sr^-1

    , m_dRayleighScatteringScale(0.033100f) // km^-1
    , m_dRayleighScatteringCoefficient(0.175287f, 0.409607f, 1.000000f) // unitless
    , m_dRayleighExponentialDistribution(8.000000f) // km

    , m_dMieScatteringScale(0.003996f) // km^-1
    , m_dMieScatteringCoefficient(1.000000f, 1.000000f, 1.000000f) // unitless
    , m_dMieAbsorptionScale(0.000444f) // km^-1
    , m_dMieAbsorptionCoefficient(1.000000f, 1.000000f, 1.000000f) // unitless
    , m_dMiePhaseFunctionG(0.800000f) // unitless
    , m_dMieExponentialDistribution(1.200000f) // km

    , m_dOzoneAbsorptionScale(0.001881f) // km^-1
    , m_dOzoneAbsorptionCoefficient(0.345561f, 1.000000f, 0.045189f) // unitless

    , m_notAppliedChanges(false)
    , m_mesh("D:/Escritorio/human.glb")
    , m_bulbMesh("D:/Escritorio/sphere1.glb")
    , m_groundMesh("D:/Escritorio/plane50x50.glb")
    , m_fullScreenQuadMesh("D:/Escritorio/FullScreenQuad.glb")
{
    Init();
}

void PhysicalSky::Init()
{
    ResetDefaults();
    InitModel();
    InitResources();
}

void PhysicalSky::MakeDefaultParametersNew()
{
    m_nPlanetRadius = m_dPlanetRadius;
    m_nAtmosphereHeight = m_dAtmosphereHeight;
    m_nGroundAlbedo = m_dGroundAlbedo;

    m_nSunSizeMultiplier = m_dSunSizeMultiplier;
    m_nSunIrradiance = m_dSunIrradiance;

    m_nMoonSizeMultiplier = m_dMoonSizeMultiplier;

    m_nRayleighScatteringScale = m_dRayleighScatteringScale;
    m_nRayleighScatteringCoefficient = m_dRayleighScatteringCoefficient;
    m_nRayleighExponentialDistribution = m_dRayleighExponentialDistribution;

    m_nMieScatteringScale = m_dMieScatteringScale;
    m_nMieScatteringCoefficient = m_dMieScatteringCoefficient;
    m_nMieAbsorptionScale = m_dMieAbsorptionScale;
    m_nMieAbsorptionCoefficient = m_dMieAbsorptionCoefficient;
    m_nMieExponentialDistribution = m_dMieExponentialDistribution;
    m_nMiePhaseFunctionG = m_dMiePhaseFunctionG;

    m_nOzoneAbsorptionScale = m_dOzoneAbsorptionScale;
    m_nOzoneAbsorptionCoefficient = m_dOzoneAbsorptionCoefficient;
}

void PhysicalSky::MakeNewParametersCurrent()
{
    m_cPlanetRadius = m_nPlanetRadius;
    m_cAtmosphereHeight = m_nAtmosphereHeight;
    m_cGroundAlbedo = m_nGroundAlbedo;

    m_cSunSizeMultiplier = m_nSunSizeMultiplier;
    m_cSunIrradiance = m_nSunIrradiance;

    m_cMoonSizeMultiplier = m_nMoonSizeMultiplier;

    m_cRayleighScatteringScale = m_nRayleighScatteringScale;
    m_cRayleighScatteringCoefficient = m_nRayleighScatteringCoefficient;
    m_cRayleighExponentialDistribution = m_nRayleighExponentialDistribution;

    m_cMieScatteringScale = m_nMieScatteringScale;
    m_cMieScatteringCoefficient = m_nMieScatteringCoefficient;
    m_cMieAbsorptionScale = m_nMieAbsorptionScale;
    m_cMieAbsorptionCoefficient = m_nMieAbsorptionCoefficient;
    m_cMieExponentialDistribution = m_nMieExponentialDistribution;
    m_cMiePhaseFunctionG = m_nMiePhaseFunctionG;

    m_cOzoneAbsorptionScale = m_nOzoneAbsorptionScale;
    m_cOzoneAbsorptionCoefficient = m_nOzoneAbsorptionCoefficient;
}

void PhysicalSky::ResetDefaults()
{
    MakeDefaultParametersNew();

    MakeNewParametersCurrent();

    m_cSunLimbDarkeningAlgorithm = m_dSunLimbDarkeningAlgorithm;

    m_cMoonEarthshineEnable = m_dMoonEarthshineEnable;
    m_cMoonColorMapEnable = m_dMoonColorMapEnable;
    m_cMoonNormalMapStrength = m_dMoonNormalMapStrength;

    m_cSkyStarsMapMultiplier = m_dSkyStarsMapMultiplier;
    m_cSkyMilkywayMapMultiplier = m_dSkyMilkywayMapMultiplier;

    m_cArtificialLightEnable = m_dArtificialLightEnable;
    m_cArtificialLightPos = m_dArtificialLightPos;
    m_cArtificialLightRadiantIntensity = m_dArtificialLightRadiantIntensity;
}

bool PhysicalSky::AnyChange()
{
    bool result = false;

    result |= m_nPlanetRadius != m_dPlanetRadius;
    result |= m_nAtmosphereHeight != m_dAtmosphereHeight;
    result |= m_nGroundAlbedo != m_dGroundAlbedo;

    result |= m_nSunSizeMultiplier != m_dSunSizeMultiplier;
    result |= m_nSunIrradiance != m_dSunIrradiance;

    result |= m_nMoonSizeMultiplier != m_dMoonSizeMultiplier;

    result |= m_nRayleighScatteringScale != m_dRayleighScatteringScale;
    result |= m_nRayleighScatteringCoefficient != m_dRayleighScatteringCoefficient;
    result |= m_nRayleighExponentialDistribution != m_dRayleighExponentialDistribution;

    result |= m_nMieScatteringScale != m_dMieScatteringScale;
    result |= m_nMieScatteringCoefficient != m_dMieScatteringCoefficient;
    result |= m_nMieAbsorptionScale != m_dMieAbsorptionScale;
    result |= m_nMieAbsorptionCoefficient != m_dMieAbsorptionCoefficient;
    result |= m_nMieExponentialDistribution != m_dMieExponentialDistribution;
    result |= m_nMiePhaseFunctionG != m_dMiePhaseFunctionG;

    result |= m_nOzoneAbsorptionScale != m_dOzoneAbsorptionScale;
    result |= m_nOzoneAbsorptionCoefficient != m_dOzoneAbsorptionCoefficient;


    result |= m_cSunLimbDarkeningAlgorithm != m_dSunLimbDarkeningAlgorithm;

    result |= m_cMoonEarthshineEnable != m_dMoonEarthshineEnable;
    result |= m_cMoonColorMapEnable != m_dMoonColorMapEnable;
    result |= m_cMoonNormalMapStrength != m_dMoonNormalMapStrength;

    result |= m_cSkyStarsMapMultiplier != m_dSkyStarsMapMultiplier;
    result |= m_cSkyMilkywayMapMultiplier != m_dSkyMilkywayMapMultiplier;

    result |= m_cArtificialLightEnable != m_dArtificialLightEnable;
    result |= m_cArtificialLightPos != m_dArtificialLightPos;
    result |= m_cArtificialLightRadiantIntensity != m_dArtificialLightRadiantIntensity;

    return result;
}

glm::dvec3 PhysicalSky::ComputeMoonIrradiance()
{
    constexpr double C = 0.072;
    constexpr double r_m = 1.162671e-5; // Radius of the moon (in AU)
    double d = m_astronomicalPositioning.GetMoonHorizonCoordinates().z; // Earth-Moon distance (in AU)
    double E_sm = static_cast<double>(m_cSunIrradiance);
    double E_em = ComputeEarthshineIrradiance();
    double phi = m_astronomicalPositioning.GetMoonPhaseAngle();
    double moonLitFraction = VisibleLitFractionFromPhaseAngle(phi);
    double q = r_m / d;
    double E_m = (2.0 / 3.0)*C*q*q * (E_em + E_sm * moonLitFraction);
    return glm::dvec3(1.0, 1.0, 1.0) * (E_m / 3.0);
}

double PhysicalSky::ComputeEarthshineIrradiance()
{
    double phi = m_astronomicalPositioning.GetEarthPhaseAngle();
    double earthLitFraction = VisibleLitFractionFromPhaseAngle(phi);
    constexpr double fullEarthshineIrradiance = 0.19;
    double earthshineIrradiance = 0.5 * fullEarthshineIrradiance * earthLitFraction;
    return earthshineIrradiance;
}

void PhysicalSky::InitModel()
{
    int viewportData[4];
    glGetIntegerv(GL_VIEWPORT, viewportData);

    glm::dvec3 sun_irradiance = glm::dvec3(1.0, 1.0, 1.0) * (static_cast<double>(m_cSunIrradiance) / 3.0);
    glm::vec3 sunHorizonCoordinates = m_astronomicalPositioning.GetSunHorizonCoordinates();
    constexpr double sunRadius = 0.00465047;
    double sunTanAngularRadius = (sunRadius * m_cSunSizeMultiplier)/ sunHorizonCoordinates.z;
    double sun_angular_radius = glm::atan(sunTanAngularRadius);

    glm::dvec3 moon_irradiance = ComputeMoonIrradiance();
    glm::vec3 moonHorizonCoordinates = m_astronomicalPositioning.GetMoonHorizonCoordinates();
    constexpr double moonRadius = 0.00001163;
    double moonTanAngularRadius = (moonRadius * m_cMoonSizeMultiplier) / moonHorizonCoordinates.z;
    double moon_angular_radius = glm::atan(moonTanAngularRadius);

    double bottom_radius = static_cast<double>(m_cPlanetRadius) * 1000.0;
    double top_radius = bottom_radius + static_cast<double>(m_cAtmosphereHeight) * 1000.0;

    DensityProfileLayer rayleigh_layer(0.0, 1.0, -1.0 / (static_cast<double>(m_cRayleighExponentialDistribution) * 1000.0), 0.0, 0.0);
    glm::dvec3 rayleigh_scattering = static_cast<glm::dvec3>(m_cRayleighScatteringCoefficient) * (static_cast<double>(m_cRayleighScatteringScale) / 1000.0);

    DensityProfileLayer mie_layer(0.0, 1.0, -1.0 / (static_cast<double>(m_cMieExponentialDistribution) * 1000.0), 0.0, 0.0);
    glm::dvec3 mie_scattering = static_cast<glm::dvec3>(m_cMieScatteringCoefficient) * (static_cast<double>(m_cMieScatteringScale) / 1000.0);
    glm::dvec3 mie_extinction = mie_scattering + static_cast<glm::dvec3>(m_cMieAbsorptionCoefficient) * (static_cast<double>(m_cMieAbsorptionScale) / 1000.0);
    double mie_phase_function_g = static_cast<double>(m_cMiePhaseFunctionG);

    // Density profile increasing linearly from 0 to 1 between 10 and 25km, and
    // decreasing linearly from 1 to 0 between 25 and 40km. This is an approximate
    // profile from http://www.kln.ac.lk/science/Chemistry/Teaching_Resources/
    // Documents/Introduction%20to%20atmospheric%20chemistry.pdf (page 10).
    std::vector<DensityProfileLayer> ozone_density;
    ozone_density.push_back(DensityProfileLayer(25000.0, 0.0, 0.0, 1.0 / 15000.0, -2.0 / 3.0));
    ozone_density.push_back(DensityProfileLayer(0.0, 0.0, 0.0, -1.0 / 15000.0, 8.0 / 3.0));
    glm::dvec3 absorption_extinction = (static_cast<glm::dvec3>(m_cOzoneAbsorptionCoefficient) * static_cast<double>(m_cOzoneAbsorptionScale)) / 1000.0;

    glm::dvec3 ground_albedo = static_cast<glm::dvec3>(m_cGroundAlbedo);
    constexpr double max_sun_zenith_angle = 120.0 / 180.0 * glm::pi<double>(); // TODO: Take a look at this value https://ebruneton.github.io/precomputed_atmospheric_scattering/atmosphere/model.h.html

    m_solarModel.reset(new Model(sun_irradiance, sun_angular_radius, moon_irradiance, moon_angular_radius,
        bottom_radius, top_radius, { rayleigh_layer }, rayleigh_scattering,
        { mie_layer }, mie_scattering, mie_extinction, mie_phase_function_g,
        ozone_density, absorption_extinction, ground_albedo, max_sun_zenith_angle,
        kLengthUnitInMeters, SOURCE_SUN));
    m_solarModel->Init();

    m_lunarModel.reset(new Model(sun_irradiance, sun_angular_radius, moon_irradiance, moon_angular_radius,
        bottom_radius, top_radius, { rayleigh_layer }, rayleigh_scattering,
        { mie_layer }, mie_scattering, mie_extinction, mie_phase_function_g,
        ozone_density, absorption_extinction, ground_albedo, max_sun_zenith_angle,
        kLengthUnitInMeters, SOURCE_MOON));
    m_lunarModel->Init();

    glViewport(viewportData[0], viewportData[1], viewportData[2], viewportData[3]);

    if (glGetError() != GL_NO_ERROR) std::cerr << "[OpenGL] E: Initializing model." << std::endl;

    /*
    <p>Then, it creates and compiles the vertex and fragment shaders used to render
    our demo scene, and link them with the <code>Model</code>'s atmosphere shader
    to get the final scene rendering program:
    */
    InitShaders();

    // Required because initalizing the model messes with these
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void PhysicalSky::InitShaders()
{
    ShaderStage skyVertexShader = ShaderStage();
    skyVertexShader.Create(ShaderType::VERTEX);
    skyVertexShader.Compile("D:/dev/miri-tfm/resources/shaders/sky.vert", "D:/dev/miri-tfm/resources/shaders/");
    ShaderStage skyFragmentShader = ShaderStage();
    skyFragmentShader.Create(ShaderType::FRAGMENT);
    skyFragmentShader.Compile("D:/dev/miri-tfm/resources/shaders/sky.frag", "D:/dev/miri-tfm/resources/shaders/");
    m_skyShader.Create();
    m_skyShader.AttachShader(skyVertexShader.m_id);
    m_skyShader.AttachShader(skyFragmentShader.m_id);
    m_skyShader.AttachShader(m_solarModel->shader());
    m_skyShader.Build();

    ShaderStage moonVertexShader = ShaderStage();
    moonVertexShader.Create(ShaderType::VERTEX);
    moonVertexShader.Compile("D:/dev/miri-tfm/resources/shaders/moon.vert", "D:/dev/miri-tfm/resources/shaders/");
    ShaderStage moonFragmentShader = ShaderStage();
    moonFragmentShader.Create(ShaderType::FRAGMENT);
    moonFragmentShader.Compile("D:/dev/miri-tfm/resources/shaders/moon.frag", "D:/dev/miri-tfm/resources/shaders/");
    m_moonShader.Create();
    m_moonShader.AttachShader(moonVertexShader.m_id);
    m_moonShader.AttachShader(moonFragmentShader.m_id);
    m_moonShader.AttachShader(m_solarModel->shader());
    m_moonShader.Build();

    ShaderStage sunVertexShader = ShaderStage();
    sunVertexShader.Create(ShaderType::VERTEX);
    sunVertexShader.Compile("D:/dev/miri-tfm/resources/shaders/sun.vert", "D:/dev/miri-tfm/resources/shaders/");
    ShaderStage sunFragmentShader = ShaderStage();
    sunFragmentShader.Create(ShaderType::FRAGMENT);
    sunFragmentShader.Compile("D:/dev/miri-tfm/resources/shaders/sun.frag", "D:/dev/miri-tfm/resources/shaders/");
    m_sunShader.Create();
    m_sunShader.AttachShader(sunVertexShader.m_id);
    m_sunShader.AttachShader(sunFragmentShader.m_id);
    m_sunShader.AttachShader(m_solarModel->shader());
    m_sunShader.Build();

    ShaderStage meshVertexShader = ShaderStage();
    meshVertexShader.Create(ShaderType::VERTEX);
    meshVertexShader.Compile("D:/dev/miri-tfm/resources/shaders/mesh.vert", "D:/dev/miri-tfm/resources/shaders/");
    ShaderStage meshFragmentShader = ShaderStage();
    meshFragmentShader.Create(ShaderType::FRAGMENT);
    meshFragmentShader.Compile("D:/dev/miri-tfm/resources/shaders/mesh.frag", "D:/dev/miri-tfm/resources/shaders/");
    m_meshShader.Create();
    m_meshShader.AttachShader(meshVertexShader.m_id);
    m_meshShader.AttachShader(meshFragmentShader.m_id);
    m_meshShader.AttachShader(m_solarModel->shader());
    m_meshShader.Build();

    // NOTE: Might add atmosphere shader
    ShaderStage lightVertexShader = ShaderStage();
    lightVertexShader.Create(ShaderType::VERTEX);
    lightVertexShader.Compile("D:/dev/miri-tfm/resources/shaders/light.vert", "D:/dev/miri-tfm/resources/shaders/");
    ShaderStage lightFragmentShader = ShaderStage();
    lightFragmentShader.Create(ShaderType::FRAGMENT);
    lightFragmentShader.Compile("D:/dev/miri-tfm/resources/shaders/light.frag", "D:/dev/miri-tfm/resources/shaders/");
    m_lightShader.Create();
    m_lightShader.AttachShader(lightVertexShader.m_id);
    m_lightShader.AttachShader(lightFragmentShader.m_id);
    m_lightShader.Build();

}

void PhysicalSky::InitResources()
{
    m_moonNormalMap.Load("D:/Descargas/moon_xnormal.png");
    m_moonColorMap.Load("D:/Descargas/lroc_color_poles_8k_nogamma.png");
    m_skyStarsMap.Load("D:/Escritorio/hiptyc_2020_8k.hdr");
    m_skyMilkywayMap.Load("D:/Escritorio/milkyway_2020_8k.hdr");
}

void PhysicalSky::Update()
{
    m_astronomicalPositioning.Update();

    if (ImGui::Begin("Atmosphere Rendering Settings"))
    {
        if (ImGui::CollapsingHeader("General"))
        {
            ImGui::PushID("General");
            m_notAppliedChanges |= ImGui::SliderFloat("Planet Radius (km)", &m_nPlanetRadius, 1.0f, 10000.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            m_notAppliedChanges |= ImGui::SliderFloat("Atmosphere Height (km)", &m_nAtmosphereHeight, 1.0f, 200.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            m_notAppliedChanges |= ImGui::ColorEdit3("Ground Albedo", glm::value_ptr(m_nGroundAlbedo), ImGuiColorEditFlags_Float);
            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("Sun"))
        {
            ImGui::PushID("Sun");
            // TODO: Color
            m_notAppliedChanges |= ImGui::SliderFloat("Size Multiplier", &m_nSunSizeMultiplier, 0.2f, 5.0f);
            m_notAppliedChanges |= ImGui::SliderFloat("Irradiance (W*m^-2)", &m_nSunIrradiance, 0.0f, 15000.0f);
            ImGui::RadioButton("None", reinterpret_cast<int*>(&m_cSunLimbDarkeningAlgorithm), static_cast<int>(SunLimbDarkeningAlgorithm::NONE)); ImGui::SameLine();
            ImGui::RadioButton("NEC96", reinterpret_cast<int*>(&m_cSunLimbDarkeningAlgorithm), static_cast<int>(SunLimbDarkeningAlgorithm::NEC96)); ImGui::SameLine();
            ImGui::RadioButton("HM98", reinterpret_cast<int*>(&m_cSunLimbDarkeningAlgorithm), static_cast<int>(SunLimbDarkeningAlgorithm::HM98)); ImGui::SameLine();
            ImGui::RadioButton("Invalid", reinterpret_cast<int*>(&m_cSunLimbDarkeningAlgorithm), -1);
            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("Moon"))
        {
            ImGui::PushID("Moon");
            m_notAppliedChanges |= ImGui::SliderFloat("Size Multiplier", &m_nMoonSizeMultiplier, 0.2f, 5.0f);
            ImGui::Checkbox("Enable Earthshine", &m_cMoonEarthshineEnable);
            ImGui::Checkbox("Use Color Map", &m_cMoonColorMapEnable);
            ImGui::SliderFloat("Normal Map Strength", &m_cMoonNormalMapStrength, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("Sky"))
        {
            ImGui::PushID("Sky");
            ImGui::SliderFloat("Stars Map Multiplier", &m_cSkyStarsMapMultiplier, -5.0f, 5.0f);
            ImGui::SliderFloat("Milky Way Map Multiplier", &m_cSkyMilkywayMapMultiplier, -5.0f, 5.0f);
            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("Artifical Light"))
        {
            ImGui::PushID("Artifical Light");
            ImGui::Checkbox("Enable", &m_cArtificialLightEnable);
            ImGui::DragFloat3("Position (m)", glm::value_ptr(m_cArtificialLightPos), 0.01f);
            ImGui::SliderFloat("Radiant Intensity (W*sr^-1)", &m_cArtificialLightRadiantIntensity, 0.0f, 50.0, "%.3f");
            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("Rayleigh"))
        {
            ImGui::PushID("Rayleigh");
            m_notAppliedChanges |= ImGui::SliderFloat("Scattering Scale (km^-1)", &m_nRayleighScatteringScale, 0.0f, 2.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            m_notAppliedChanges |= ImGui::ColorEdit3("Scattering Coefficient", glm::value_ptr(m_nRayleighScatteringCoefficient), ImGuiColorEditFlags_Float);
            m_notAppliedChanges |= ImGui::SliderFloat("Exponential Distribution (km)", &m_nRayleighExponentialDistribution, 0.5f, 20.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("Mie"))
        {
            ImGui::PushID("Mie");
            m_notAppliedChanges |= ImGui::SliderFloat("Scattering Scale (km^-1)", &m_nMieScatteringScale, 0.0f, 10.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            m_notAppliedChanges |= ImGui::ColorEdit3("Scattering Coefficient", glm::value_ptr(m_nMieScatteringCoefficient), ImGuiColorEditFlags_Float);
            m_notAppliedChanges |= ImGui::SliderFloat("Absorption Scale (km^-1)", &m_nMieAbsorptionScale, 0.0f, 10.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            m_notAppliedChanges |= ImGui::ColorEdit3("Absorption Coefficient", glm::value_ptr(m_nMieAbsorptionCoefficient), ImGuiColorEditFlags_Float);
            m_notAppliedChanges |= ImGui::SliderFloat("Exponential Distribution (km)", &m_nMieExponentialDistribution, 0.5f, 20.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            m_notAppliedChanges |= ImGui::SliderFloat("Phase Function g", &m_nMiePhaseFunctionG, 0.0f, 1.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("Ozone"))
        {
            ImGui::PushID("Ozone");
            m_notAppliedChanges |= ImGui::SliderFloat("Absorption Scale (km^-1)", &m_nOzoneAbsorptionScale, 0.0f, 10.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            m_notAppliedChanges |= ImGui::ColorEdit3("Absorption Coefficient", glm::value_ptr(m_nOzoneAbsorptionCoefficient), ImGuiColorEditFlags_Float);
            ImGui::PopID();
        }

        if (AnyChange())
        {
            if (ImGui::Button("Reset Defaults"))
            {
                ResetDefaults();
                InitModel();
            }
        }

        if (m_notAppliedChanges)
        {
            if (ImGui::Button("Recompute Model"))
            {
                MakeNewParametersCurrent();
                InitModel();
                m_notAppliedChanges = false;
            }
        }
    }
    ImGui::End();
}

void PhysicalSky::Render(const Camera& camera)
{
    glm::mat4 horizonToWorld = glm::mat4(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    glm::dvec3 sunHorizonCoordinates = m_astronomicalPositioning.GetSunHorizonCoordinates();
    glm::vec2 sunHorizonCos = glm::cos(sunHorizonCoordinates);
    glm::vec2 sunHorizonSin = glm::sin(sunHorizonCoordinates);
    glm::vec3 sunHorizonDirection = glm::vec3(sunHorizonCos.y * sunHorizonCos.x, sunHorizonCos.y * sunHorizonSin.x, sunHorizonSin.y);
    glm::vec3 sunWorldDirection = glm::vec3(horizonToWorld * glm::vec4(sunHorizonDirection, 0.0));
    constexpr float sunRadius = 0.00465047f;
    float tanSunAngularRadius = (m_cSunSizeMultiplier * sunRadius) / sunHorizonCoordinates.z;

    glm::dvec3 moonHorizonCoordinates = m_astronomicalPositioning.GetMoonHorizonCoordinates();
    glm::vec2 moonHorizonCos = glm::cos(moonHorizonCoordinates);
    glm::vec2 moonHorizonSin = glm::sin(moonHorizonCoordinates);
    glm::vec3 moonHorizonDirection = glm::vec3(moonHorizonCos.y * moonHorizonCos.x, moonHorizonCos.y * moonHorizonSin.x, moonHorizonSin.y);
    glm::vec3 moonWorldDirection = glm::vec3(horizonToWorld * glm::vec4(moonHorizonDirection, 0.0));
    constexpr float moonRadius = 0.00001163f;
    float tanMoonAngularRadius = (m_cMoonSizeMultiplier * moonRadius) / moonHorizonCoordinates.z;

    glDisable(GL_DEPTH_TEST);
    {
        RenderSky(camera, sunWorldDirection, moonWorldDirection);
        RenderSun(camera, sunWorldDirection, moonWorldDirection, tanSunAngularRadius);
        RenderMoon(camera, sunWorldDirection, moonWorldDirection, tanMoonAngularRadius);
    }
    glEnable(GL_DEPTH_TEST);

    RenderScene(camera, sunWorldDirection, moonWorldDirection);
    if (m_cArtificialLightEnable) RenderLight(camera);
}

void PhysicalSky::RenderSun(const Camera& camera, const glm::vec3& sunWorldDirection, const glm::vec3& moonWorldDirection, float tanSunAngularRadius)
{
    m_sunShader.Use();
    m_solarModel->SetProgramUniforms(m_sunShader.m_id, 0, 1, 2, 3);
    m_lunarModel->SetProgramUniforms(m_sunShader.m_id, 4, 5, 6, 7);

    glm::mat4 sunBillboardModel = BillboardModelFromCamera(camera.GetPosition(), sunWorldDirection);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(tanSunAngularRadius));

    m_sunShader.SetMat4("Model", sunBillboardModel * scale);
    m_sunShader.SetMat4("View", camera.GetViewMatrix());
    m_sunShader.SetMat4("Projection", camera.GetProjectionMatrix());

    m_sunShader.SetVec3("w_CameraPos", camera.GetPosition());
    m_sunShader.SetVec3("w_EarthCenterPos", glm::vec3(0.0f, -m_cPlanetRadius, 0.0f));
    m_sunShader.SetVec3("w_SunDir", sunWorldDirection);
    m_sunShader.SetVec3("w_MoonDir", moonWorldDirection);

    m_sunShader.SetInt("LimbDarkeningAlgorithm", static_cast<int>(m_cSunLimbDarkeningAlgorithm));

    m_fullScreenQuadMesh.Render();
}

void PhysicalSky::RenderMoon(const Camera& camera, const glm::vec3& sunWorldDirection, const glm::vec3& moonWorldDirection, float tanMoonAngularRadius)
{
    m_moonShader.Use();
    m_solarModel->SetProgramUniforms(m_moonShader.m_id, 0, 1, 2, 3);
    m_lunarModel->SetProgramUniforms(m_moonShader.m_id, 4, 5, 6, 7);

    glm::mat4 moonBillboardModel = BillboardModelFromCamera(camera.GetPosition(), moonWorldDirection);
    glm::mat4 moonScale = glm::scale(glm::mat4(1.0f), glm::vec3(tanMoonAngularRadius));

    m_moonShader.SetMat4("Model", moonBillboardModel * moonScale);
    m_moonShader.SetMat4("View", camera.GetViewMatrix());
    m_moonShader.SetMat4("Projection", camera.GetProjectionMatrix());

    m_moonShader.SetVec3("w_CameraPos", camera.GetPosition());
    m_moonShader.SetVec3("w_PlanetPos", glm::vec3(0.0f, -m_cPlanetRadius, 0.0f));
    m_moonShader.SetVec3("w_SunDir", sunWorldDirection);
    m_moonShader.SetVec3("w_MoonDir", moonWorldDirection);
    m_moonShader.SetVec3("w_EarthDir", -moonWorldDirection);

    double earthshineIrradiance = ComputeEarthshineIrradiance();
    m_moonShader.SetFloat("EarthIrradiance", static_cast<float>(earthshineIrradiance));
    m_moonShader.SetFloat("SunIrradiance", m_cSunIrradiance);
    m_moonShader.SetTexture("ColorMap", 8, m_moonColorMap);
    m_moonShader.SetTexture("NormalMap", 9, m_moonNormalMap);
    m_moonShader.SetFloat("NormalMapStrength", m_cMoonNormalMapStrength);
    m_moonShader.SetBool("UseColorMap", m_cMoonColorMapEnable);
    m_moonShader.SetBool("EnableEarthshine", m_cMoonEarthshineEnable);

    m_fullScreenQuadMesh.Render();
}

void PhysicalSky::RenderSky(const Camera& camera, const glm::vec3& sunWorldDirection, const glm::vec3& moonWorldDirection)
{
    m_skyShader.Use();
    m_solarModel->SetProgramUniforms(m_skyShader.m_id, 0, 1, 2, 3);
    m_lunarModel->SetProgramUniforms(m_skyShader.m_id, 4, 5, 6, 7);

    m_skyShader.SetMat4("WorldFromView", camera.GetWorldFromViewMatrix());
    m_skyShader.SetMat4("ViewFromClip", camera.GetViewFromClipMatrix());

    m_skyShader.SetVec3("w_CameraPos", camera.GetPosition());
    m_skyShader.SetVec3("w_EarthCenterPos", glm::vec3(0.0f, -m_cPlanetRadius, 0.0f));
    m_skyShader.SetVec3("w_SunDir", sunWorldDirection);
    m_skyShader.SetVec3("w_MoonDir", moonWorldDirection);

    m_skyShader.SetTexture("StarsMap", 8, m_skyStarsMap);
    m_skyShader.SetFloat("StarsMapMultiplier", glm::pow(10.0f, m_cSkyStarsMapMultiplier));

    m_skyShader.SetTexture("MilkywayMap", 9, m_skyMilkywayMap);
    m_skyShader.SetFloat("MilkywayMapMultiplier", glm::pow(10.0f, m_cSkyMilkywayMapMultiplier));

    m_skyShader.SetFloat("lon", m_astronomicalPositioning.GetLon());
    m_skyShader.SetFloat("lat", m_astronomicalPositioning.GetLat());
    m_skyShader.SetFloat("T", m_astronomicalPositioning.GetT());

    m_fullScreenQuadMesh.Render();
}

void PhysicalSky::RenderScene(const Camera& camera, const glm::vec3& sunWorldDirection, const glm::vec3& moonWorldDirection)
{
    m_meshShader.Use();
    m_solarModel->SetProgramUniforms(m_meshShader.m_id, 0, 1, 2, 3);
    m_lunarModel->SetProgramUniforms(m_meshShader.m_id, 4, 5, 6, 7);

    m_meshShader.SetMat4("Model", glm::scale(glm::mat4(1.0f), glm::vec3(1e-3f)));
    m_meshShader.SetMat4("View", camera.GetViewMatrix());
    m_meshShader.SetMat4("Projection", camera.GetProjectionMatrix());

    m_meshShader.SetVec3("w_CameraPos", camera.GetPosition());
    m_meshShader.SetVec3("w_EarthCenterPos", glm::vec3(0.0f, -m_cPlanetRadius, 0.0f));
    m_meshShader.SetVec3("w_SunDir", sunWorldDirection);
    m_meshShader.SetVec3("w_MoonDir", moonWorldDirection);

    m_meshShader.SetVec3("w_LightPos", m_cArtificialLightPos / 1000.0f);
    m_meshShader.SetVec3("LightRadiantIntensity", glm::vec3(1.0f) * (m_cArtificialLightRadiantIntensity / 3.0f));
    m_meshShader.SetBool("EnableLight", m_cArtificialLightEnable);

    m_mesh.Render();

    m_meshShader.SetMat4("Model", glm::scale(glm::mat4(1.0f), glm::vec3(1e-3f)));
    m_groundMesh.Render();
}

void PhysicalSky::RenderLight(const Camera& camera)
{
    m_lightShader.Use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1e-3f));
    model = glm::translate(model, m_cArtificialLightPos);
    model = glm::scale(model, glm::vec3(0.2f));
    m_lightShader.SetMat4("Model", model);
    m_lightShader.SetMat4("View", camera.GetViewMatrix());
    m_lightShader.SetMat4("Projection", camera.GetProjectionMatrix());

    m_bulbMesh.Render();
}

glm::mat4 PhysicalSky::BillboardModelFromCamera(const glm::vec3& cameraPosition, const glm::vec3& billboardDirection)
{
    glm::vec3 position = cameraPosition + billboardDirection;
    glm::vec3 forward = glm::normalize(billboardDirection);
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::cross(right, forward);
    glm::mat4 model = glm::mat4(glm::vec4(right, 0.0f), glm::vec4(up, 0.0f), glm::vec4(-forward, 0.0), glm::vec4(position, 1.0));
    return model;
}

double PhysicalSky::VisibleLitFractionFromPhaseAngle(double phi)
{
    return 1.0 - glm::sin(0.5 * phi) * glm::tan(0.5 * phi) * glm::log(1.0 / glm::tan(0.25 * phi));
}