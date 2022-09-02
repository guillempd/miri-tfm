#include "PhysicalSky.h"

#include <imgui.h>

#include <glm/gtc/type_ptr.hpp>

#include <iostream>

namespace {
    constexpr double kLengthUnitInMeters = 1000.0;
}  // anonymous namespace

using namespace atmosphere;

// TODO: Put units in the ImGui interface
PhysicalSky::PhysicalSky()
    : m_dRayleighScatteringScale(0.033100f) // km^-1
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
    , m_dGroundAlbedo(0.401978f, 0.401978f, 0.401978f) // unitless
    , m_dSunIntensity(1500.000000f) // W*m^-2
    , m_dSunAngularRadius(0.05f) // rad // Correct value is 0.004675f
    , m_dMoonAngularRadius(0.05f) // rad // TODO: Get correct value
    , m_dPlanetRadius(6360.0f) // km
    , m_dAtmosphereHeight(60.0f) // km

    , m_dLimbDarkeningAlgorithm(LimbDarkeningAlgorithm::NONE)
    , m_notAppliedChanges(false)
    , m_mesh("D:/Escritorio/Monkey.glb")
    , m_starsMapIntensity(0.0f)
    , m_dMoonNormalMapStrength(0.5f)
    , m_dMoonUseColorMap(true)
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
    m_nGroundAlbedo = m_dGroundAlbedo;
    m_nSunIntensity = m_dSunIntensity;
    m_nSunAngularRadius = m_dSunAngularRadius;
    m_nPlanetRadius = m_dPlanetRadius;
    m_nAtmosphereHeight = m_dAtmosphereHeight;
    m_nRayleighScatteringCoefficient = m_dRayleighScatteringCoefficient;
    m_nRayleighScatteringScale = m_dRayleighScatteringScale;
    m_nRayleighExponentialDistribution = m_dRayleighExponentialDistribution;
    m_nMieScatteringCoefficient = m_dMieScatteringCoefficient;
    m_nMieScatteringScale = m_dMieScatteringScale;
    m_nMieAbsorptionCoefficient = m_dMieAbsorptionCoefficient;
    m_nMieAbsorptionScale = m_dMieAbsorptionScale;
    m_nMiePhaseFunctionG = m_dMiePhaseFunctionG;
    m_nMieExponentialDistribution = m_dMieExponentialDistribution;
    m_nOzoneAbsorptionCoefficient = m_dOzoneAbsorptionCoefficient;
    m_nOzoneAbsorptionScale = m_dOzoneAbsorptionScale;
    m_nMoonAngularRadius = m_dMoonAngularRadius;
}

void PhysicalSky::MakeNewParametersCurrent()
{
    m_cGroundAlbedo = m_nGroundAlbedo;
    m_cSunIntensity = m_nSunIntensity;
    m_cSunAngularRadius = m_nSunAngularRadius;
    m_cPlanetRadius = m_nPlanetRadius;
    m_cAtmosphereHeight = m_nAtmosphereHeight;
    m_cRayleighScatteringCoefficient = m_nRayleighScatteringCoefficient;
    m_cRayleighScatteringScale = m_nRayleighScatteringScale;
    m_cRayleighExponentialDistribution = m_nRayleighExponentialDistribution;
    m_cMieScatteringCoefficient = m_nMieScatteringCoefficient;
    m_cMieScatteringScale = m_nMieScatteringScale;
    m_cMieAbsorptionCoefficient = m_nMieAbsorptionCoefficient;
    m_cMieAbsorptionScale = m_nMieAbsorptionScale;
    m_cMiePhaseFunctionG = m_nMiePhaseFunctionG;
    m_cMieExponentialDistribution = m_nMieExponentialDistribution;
    m_cOzoneAbsorptionCoefficient = m_nOzoneAbsorptionCoefficient;
    m_cOzoneAbsorptionScale = m_nOzoneAbsorptionScale;
    m_cMoonAngularRadius = m_nMoonAngularRadius;
}

void PhysicalSky::ResetDefaults()
{
    MakeDefaultParametersNew();
    MakeNewParametersCurrent();
    m_cLimbDarkeningAlgorithm = m_dLimbDarkeningAlgorithm;
    m_cMoonNormalMapStrength = m_dMoonNormalMapStrength;
    m_cMoonUseColorMap = m_dMoonUseColorMap;
}

bool PhysicalSky::AnyChange()
{
    bool result = false;

    result |= m_nGroundAlbedo != m_dGroundAlbedo;
    result |= m_nSunIntensity != m_dSunIntensity;
    result |= m_nSunAngularRadius != m_dSunAngularRadius;
    result |= m_nPlanetRadius != m_dPlanetRadius;
    result |= m_nAtmosphereHeight != m_dAtmosphereHeight;
    result |= m_nRayleighScatteringCoefficient != m_dRayleighScatteringCoefficient;
    result |= m_nRayleighScatteringScale != m_dRayleighScatteringScale;
    result |= m_nRayleighExponentialDistribution != m_dRayleighExponentialDistribution;
    result |= m_nMieScatteringCoefficient != m_dMieScatteringCoefficient;
    result |= m_nMieScatteringScale != m_dMieScatteringScale;
    result |= m_nMieAbsorptionCoefficient != m_dMieAbsorptionCoefficient;
    result |= m_nMieAbsorptionScale != m_dMieAbsorptionScale;
    result |= m_nMiePhaseFunctionG != m_dMiePhaseFunctionG;
    result |= m_nMieExponentialDistribution != m_dMieExponentialDistribution;
    result |= m_nOzoneAbsorptionCoefficient != m_dOzoneAbsorptionCoefficient;
    result |= m_nOzoneAbsorptionScale != m_dOzoneAbsorptionScale;
    result |= m_nMoonAngularRadius != m_dMoonAngularRadius;

    result |= m_cLimbDarkeningAlgorithm != m_dLimbDarkeningAlgorithm;
    result |= m_cMoonNormalMapStrength != m_dMoonNormalMapStrength;
    result |= m_cMoonUseColorMap != m_dMoonUseColorMap;

    return result;
}

// TODO: r_m and d in other units
glm::dvec3 PhysicalSky::ComputeMoonIrradiance()
{
    double C = 0.072;
    double r_m = 1737.4; // Radius of the moon in km
    double d = 384400.0; // Earth-Moon distance in km // TODO: Get from m_coordinates (in AU)
    double E_sm = 1500.0;
    double E_em = 0.19 * 0.5; // TODO: Compute correctly
    double phi = m_coordinates.GetMoonPhaseAngle();
    double moonLitFraction = VisibleLitFractionFromPhaseAngle(phi);
    double q = r_m / d;
    double E_m = (2.0 / 3.0)*C*q*q * (E_em + 2.0 * E_sm * moonLitFraction);
    return glm::dvec3(1.0, 1.0, 1.0) * (E_m / 3.0);
}

void PhysicalSky::InitModel()
{
    int viewportData[4];
    glGetIntegerv(GL_VIEWPORT, viewportData);

    glm::dvec3 sun_irradiance = glm::dvec3(1.0, 1.0, 1.0) * (static_cast<double>(m_cSunIntensity) / 3.0);
    double sun_angular_radius = static_cast<double>(m_cSunAngularRadius);

    glm::dvec3 moon_irradiance = ComputeMoonIrradiance();
    double moon_angular_radius = static_cast<double>(m_cMoonAngularRadius);

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
    const double max_sun_zenith_angle = 120.0 / 180.0 * glm::pi<double>(); // TODO: Take a look at this value https://ebruneton.github.io/precomputed_atmospheric_scattering/atmosphere/model.h.html

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
}

void PhysicalSky::InitResources()
{
    glGenVertexArrays(1, &m_fullScreenQuadVao);
    glBindVertexArray(m_fullScreenQuadVao);
    glGenBuffers(1, &m_fullScreenQuadVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullScreenQuadVbo);
    const GLfloat vertices[] = {
        -1.0, -1.0, 0.0, 1.0,
        +1.0, -1.0, 0.0, 1.0,
        -1.0, +1.0, 0.0, 1.0,
        +1.0, +1.0, 0.0, 1.0
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
    constexpr GLuint kAttribIndex = 0;
    constexpr int kCoordsPerVertex = 4;
    glVertexAttribPointer(kAttribIndex, kCoordsPerVertex, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(kAttribIndex);
    glBindVertexArray(GL_NONE);

    m_moonNormalMap.Load("D:/Descargas/moon_xnormal.png");
    m_moonColorMap.Load("D:/Descargas/lroc_color_poles_8k_nogamma.png");
    m_starsMap.Load("D:/Escritorio/starmap_2020_8k.hdr");
}

PhysicalSky::~PhysicalSky()
{
    glDeleteBuffers(1, &m_fullScreenQuadVbo);
    glDeleteVertexArrays(1, &m_fullScreenQuadVao);
}

void PhysicalSky::Update()
{
    m_coordinates.Update();

    if (ImGui::Begin("Atmosphere Rendering Settings"))
    {
        if (ImGui::CollapsingHeader("General"))
        {
            ImGui::PushID("General");
            ImGui::SliderFloat("Stars Map Intensity", &m_starsMapIntensity, -5.0f, 5.0f);
            m_notAppliedChanges |= ImGui::ColorEdit3("Ground Albedo", glm::value_ptr(m_nGroundAlbedo), ImGuiColorEditFlags_Float);
            m_notAppliedChanges |= ImGui::SliderFloat("Planet Radius", &m_nPlanetRadius, 1.0f, 10000.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            m_notAppliedChanges |= ImGui::SliderFloat("Atmosphere Height", &m_nAtmosphereHeight, 1.0f, 200.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("Sun"))
        {
            ImGui::PushID("Sun");
            // TODO: Color
            ImGui::RadioButton("None", reinterpret_cast<int*>(&m_cLimbDarkeningAlgorithm), static_cast<int>(LimbDarkeningAlgorithm::NONE)); ImGui::SameLine();
            ImGui::RadioButton("NEC96", reinterpret_cast<int*>(&m_cLimbDarkeningAlgorithm), static_cast<int>(LimbDarkeningAlgorithm::NEC96)); ImGui::SameLine();
            ImGui::RadioButton("HM98", reinterpret_cast<int*>(&m_cLimbDarkeningAlgorithm), static_cast<int>(LimbDarkeningAlgorithm::HM98)); ImGui::SameLine();
            ImGui::RadioButton("Invalid", reinterpret_cast<int*>(&m_cLimbDarkeningAlgorithm), -1);
            m_notAppliedChanges |= ImGui::SliderFloat("Intensity", &m_nSunIntensity, 0.0f, 15000.0f);
            m_notAppliedChanges |= ImGui::SliderFloat("Angular Radius", &m_nSunAngularRadius, 0.0f, 0.1f);
            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("Moon"))
        {
            ImGui::PushID("Moon");
            m_notAppliedChanges |= ImGui::SliderFloat("Angular Radius", &m_nMoonAngularRadius, 0.0f, 0.1f);
            ImGui::SliderFloat("Normal Map Strength", &m_cMoonNormalMapStrength, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::Checkbox("Use Color Map", &m_cMoonUseColorMap);
            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("Sky"))
        {

        }

        if (ImGui::CollapsingHeader("Rayleigh"))
        {
            ImGui::PushID("Rayleigh");
            m_notAppliedChanges |= ImGui::SliderFloat("Scattering Scale", &m_nRayleighScatteringScale, 0.0f, 2.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            m_notAppliedChanges |= ImGui::ColorEdit3("Scattering Coefficient", glm::value_ptr(m_nRayleighScatteringCoefficient), ImGuiColorEditFlags_Float);
            m_notAppliedChanges |= ImGui::SliderFloat("Exponential Distribution", &m_nRayleighExponentialDistribution, 0.5f, 20.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("Mie"))
        {
            ImGui::PushID("Mie");
            m_notAppliedChanges |= ImGui::ColorEdit3("Scattering Coefficient", glm::value_ptr(m_nMieScatteringCoefficient), ImGuiColorEditFlags_Float);
            m_notAppliedChanges |= ImGui::SliderFloat("Scattering Scale", &m_nMieScatteringScale, 0.0f, 10.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            m_notAppliedChanges |= ImGui::ColorEdit3("Absorption Coefficient", glm::value_ptr(m_nMieAbsorptionCoefficient), ImGuiColorEditFlags_Float);
            m_notAppliedChanges |= ImGui::SliderFloat("Absorption Scale", &m_nMieAbsorptionScale, 0.0f, 10.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            m_notAppliedChanges |= ImGui::SliderFloat("Phase Function G", &m_nMiePhaseFunctionG, 0.0f, 1.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            m_notAppliedChanges |= ImGui::SliderFloat("Exponential Distribution", &m_nMieExponentialDistribution, 0.5f, 20.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("Ozone"))
        {
            ImGui::PushID("Ozone");
            m_notAppliedChanges |= ImGui::ColorEdit3("Absorption Coefficient", glm::value_ptr(m_nOzoneAbsorptionCoefficient), ImGuiColorEditFlags_Float);
            m_notAppliedChanges |= ImGui::SliderFloat("Absorption Scale", &m_nOzoneAbsorptionScale, 0.0f, 10.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
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

    glm::vec3 sunHorizonCoordinates = m_coordinates.GetSunPosition();
    glm::vec2 sunHorizonCos = glm::cos(sunHorizonCoordinates);
    glm::vec2 sunHorizonSin = glm::sin(sunHorizonCoordinates);
    glm::vec3 sunHorizonDirection = glm::vec3(sunHorizonCos.y * sunHorizonCos.x, sunHorizonCos.y * sunHorizonSin.x, sunHorizonSin.y);
    glm::vec3 sunWorldDirection = glm::vec3(horizonToWorld * glm::vec4(sunHorizonDirection, 0.0));

    glm::vec3 moonHorizonCoordinates = m_coordinates.GetMoonPosition();
    glm::vec2 moonHorizonCos = glm::cos(moonHorizonCoordinates);
    glm::vec2 moonHorizonSin = glm::sin(moonHorizonCoordinates);
    glm::vec3 moonHorizonDirection = glm::vec3(moonHorizonCos.y * moonHorizonCos.x, moonHorizonCos.y * moonHorizonSin.x, moonHorizonSin.y);
    glm::vec3 moonWorldDirection = glm::vec3(horizonToWorld * glm::vec4(moonHorizonDirection, 0.0));

    glDisable(GL_DEPTH_TEST);
    {
        RenderSky(camera, sunWorldDirection, moonWorldDirection);
        RenderSun(camera, sunWorldDirection, moonWorldDirection);
        RenderMoon(camera, sunWorldDirection, moonWorldDirection);
    }
    glEnable(GL_DEPTH_TEST);

    RenderScene(camera, sunWorldDirection, moonWorldDirection);
}

void PhysicalSky::RenderSun(const Camera& camera, const glm::vec3& sunWorldDirection, const glm::vec3& moonWorldDirection)
{
    m_sunShader.Use();
    m_solarModel->SetProgramUniforms(m_sunShader.m_id, 0, 1, 2, 3);
    m_lunarModel->SetProgramUniforms(m_sunShader.m_id, 4, 5, 6, 7);

    glm::mat4 sunBillboardModel = BillboardModelFromCamera(camera.GetPosition(), sunWorldDirection);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(glm::tan(m_cSunAngularRadius)));

    m_sunShader.SetMat4("Model", sunBillboardModel * scale);
    m_sunShader.SetMat4("View", camera.GetViewMatrix());
    m_sunShader.SetMat4("Projection", camera.GetProjectionMatrix());

    m_sunShader.SetVec3("w_CameraPos", camera.GetPosition());
    m_sunShader.SetVec3("w_EarthCenterPos", glm::vec3(0.0f, -m_cPlanetRadius, 0.0f));
    m_sunShader.SetVec3("w_SunDir", sunWorldDirection);
    m_sunShader.SetVec3("w_MoonDir", moonWorldDirection);

    m_sunShader.SetInt("LimbDarkeningAlgorithm", static_cast<int>(m_cLimbDarkeningAlgorithm));

    glBindVertexArray(m_fullScreenQuadVao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void PhysicalSky::RenderMoon(const Camera& camera, const glm::vec3& sunWorldDirection, const glm::vec3& moonWorldDirection)
{
    m_moonShader.Use();
    m_solarModel->SetProgramUniforms(m_moonShader.m_id, 0, 1, 2, 3);
    m_lunarModel->SetProgramUniforms(m_moonShader.m_id, 4, 5, 6, 7);

    glm::mat4 moonBillboardModel = BillboardModelFromCamera(camera.GetPosition(), moonWorldDirection);
    glm::mat4 moonScale = glm::scale(glm::mat4(1.0f), glm::vec3(glm::tan(m_cMoonAngularRadius)));

    float phi = m_coordinates.GetEarthPhaseAngle();
    double earthLitFraction = VisibleLitFractionFromPhaseAngle(phi);
    constexpr double fullEarthshineIntensity = 0.19;
    float earthshineIntensity = static_cast<float>(fullEarthshineIntensity * earthLitFraction);

    m_moonShader.SetMat4("Model", moonBillboardModel * moonScale);
    m_moonShader.SetMat4("View", camera.GetViewMatrix());
    m_moonShader.SetMat4("Projection", camera.GetProjectionMatrix());

    m_moonShader.SetVec3("w_CameraPos", camera.GetPosition());
    m_moonShader.SetVec3("w_PlanetPos", glm::vec3(0.0f, -m_cPlanetRadius, 0.0f));
    m_moonShader.SetVec3("w_SunDir", sunWorldDirection);
    m_moonShader.SetVec3("w_MoonDir", moonWorldDirection);
    m_moonShader.SetVec3("w_EarthDir", -moonWorldDirection);

    m_moonShader.SetFloat("EarthIrradiance", earthshineIntensity);
    m_moonShader.SetFloat("SunIrradiance", m_cSunIntensity);
    m_moonShader.SetTexture("ColorMap", 8, m_moonColorMap);
    m_moonShader.SetTexture("NormalMap", 9, m_moonNormalMap);
    m_moonShader.SetFloat("NormalMapStrength", m_cMoonNormalMapStrength);
    m_moonShader.SetBool("UseColorMap", m_cMoonUseColorMap);

    glBindVertexArray(m_fullScreenQuadVao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void PhysicalSky::RenderSky(const Camera& camera, const glm::vec3& sunWorldDirection, const glm::vec3& moonWorldDirection)
{
    m_skyShader.Use();
    m_solarModel->SetProgramUniforms(m_skyShader.m_id, 0, 1, 2, 3);
    m_lunarModel->SetProgramUniforms(m_skyShader.m_id, 4, 5, 6, 7);

    m_skyShader.SetVec3("w_CameraPos", camera.GetPosition());
    m_skyShader.SetVec3("w_EarthCenterPos", glm::vec3(0.0f, -m_cPlanetRadius, 0.0f));
    m_skyShader.SetMat4("WorldFromView", camera.GetWorldFromViewMatrix());
    m_skyShader.SetMat4("ViewFromClip", camera.GetViewFromClipMatrix());

    m_skyShader.SetVec3("w_SunDir", sunWorldDirection);
    m_skyShader.SetVec3("w_MoonDir", moonWorldDirection);

    m_skyShader.SetTexture("StarsMap", 8, m_starsMap);
    m_skyShader.SetFloat("StarsMapIntensity", glm::pow(10.0f, m_starsMapIntensity));

    m_skyShader.SetFloat("lon", m_coordinates.GetLon());
    m_skyShader.SetFloat("lat", m_coordinates.GetLat());
    m_skyShader.SetFloat("T", m_coordinates.GetT());

    if (glGetError() != GL_NO_ERROR) std::cerr << "E: Setting uniforms" << std::endl;

    glBindVertexArray(m_fullScreenQuadVao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void PhysicalSky::RenderScene(const Camera& camera, const glm::vec3& sunWorldDirection, const glm::vec3& moonWorldDirection)
{
    m_meshShader.Use();
    m_solarModel->SetProgramUniforms(m_meshShader.m_id, 0, 1, 2, 3);
    m_lunarModel->SetProgramUniforms(m_meshShader.m_id, 4, 5, 6, 7);

    m_meshShader.SetMat4("Model", glm::mat4(1.0f));
    m_meshShader.SetMat4("View", camera.GetViewMatrix());
    m_meshShader.SetMat4("Projection", camera.GetProjectionMatrix());

    m_meshShader.SetVec3("w_CameraPos", camera.GetPosition());
    m_meshShader.SetVec3("w_EarthCenterPos", glm::vec3(0.0f, -m_cPlanetRadius, 0.0f));
    m_meshShader.SetVec3("w_SunDir", sunWorldDirection);
    m_meshShader.SetVec3("w_MoonDir", moonWorldDirection);

    m_mesh.Render();
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

// NOTE: Use the other formula for better numerical stability (?)
double PhysicalSky::VisibleLitFractionFromPhaseAngle(double phi)
{
    return 0.5 * (1.0 - glm::sin(0.5 * phi) * glm::tan(0.5 * phi) * glm::log(1.0 / glm::tan(0.25 * phi)));
}