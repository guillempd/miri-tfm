#include "PhysicalSky.h"

#include <imgui.h>

#include <glm/gtc/type_ptr.hpp>

#include <iostream>

namespace {
    constexpr double kPi = 3.1415926;
    //constexpr double kSunAngularRadius = 0.00935 / 2.0;
    //constexpr double kSunSolidAngle = kPi * kSunAngularRadius * kSunAngularRadius;
    constexpr double kLengthUnitInMeters = 1000.0;
}  // anonymous namespace

using namespace atmosphere;

// TODO: Put units in the ImGui interface
PhysicalSky::PhysicalSky()
    : m_dGroundAlbedo(0.401978f, 0.401978f, 0.401978f) // unitless
    , m_dSunIntensity(100000.000000f) // lux
    , m_dSunAngularRadius(0.004675f) // rad
    , m_dPlanetRadius(6360.0f) // km
    , m_dAtmosphereHeight(60.0f) // km
    , m_dRayleighScatteringCoefficient(0.175287f, 0.409607f, 1.000000f) // unitless
    , m_dRayleighScatteringScale(0.033100f) // km^-1
    , m_dRayleighExponentialDistribution(8.000000f) // km
    , m_dMieScatteringCoefficient(1.000000f, 1.000000f, 1.000000f) // unitless
    , m_dMieScatteringScale(0.003996f) // km^-1
    , m_dMieAbsorptionCoefficient(1.000000f, 1.000000f, 1.000000f) // unitless
    , m_dMieAbsorptionScale(0.000444f) // km^-1
    , m_dMiePhaseFunctionG(0.800000f) // unitless
    , m_dMieExponentialDistribution(1.200000f) // km
    , m_dOzoneAbsorptionCoefficient(0.345561f, 1.000000f, 0.045189f) // unitless
    , m_dOzoneAbsorptionScale(0.001881f) // km^-1
    , m_dLimbDarkeningStrategy(0)
    , m_shouldRecomputeModel(false)
    , use_combined_textures_(false)
    , use_half_precision_(false)
{
    Init();
}

void PhysicalSky::Init()
{
    InitNewParameters();
    InitModel();
    InitResources();
}

void PhysicalSky::InitNewParameters()
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
    m_nLimbDarkeningStrategy = m_dLimbDarkeningStrategy;
}

void PhysicalSky::InitCurrentParameters()
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
    m_cLimbDarkeningStrategy = m_nLimbDarkeningStrategy;
}

void PhysicalSky::InitModel()
{
    InitCurrentParameters();
    const double max_sun_zenith_angle =
        (use_half_precision_ ? 102.0 : 120.0) / 180.0 * kPi;

    int viewportData[4];
    glGetIntegerv(GL_VIEWPORT, viewportData);

    glm::dvec3 sun_irradiance = glm::dvec3(1.0) * static_cast<double>(m_cSunIntensity);

    DensityProfileLayer rayleigh_layer(0.0, 1.0, -1.0 / (static_cast<double>(m_cRayleighExponentialDistribution) * 1000.0), 0.0, 0.0);
    glm::dvec3 rayleigh_scattering = (static_cast<glm::dvec3>(m_cRayleighScatteringCoefficient) * static_cast<double>(m_cRayleighScatteringScale)) / 1000.0;

    DensityProfileLayer mie_layer(0.0, 1.0, -1.0 / (static_cast<double>(m_cMieExponentialDistribution) * 1000.0), 0.0, 0.0);
    glm::dvec3 mie_scattering = (static_cast<glm::dvec3>(m_cMieScatteringCoefficient) * static_cast<double>(m_cMieScatteringScale)) / 1000.0;
    glm::dvec3 mie_extinction = mie_scattering + (static_cast<glm::dvec3>(m_cMieAbsorptionCoefficient) * static_cast<double>(m_cMieAbsorptionScale)) / 1000.0;

    // Density profile increasing linearly from 0 to 1 between 10 and 25km, and
    // decreasing linearly from 1 to 0 between 25 and 40km. This is an approximate
    // profile from http://www.kln.ac.lk/science/Chemistry/Teaching_Resources/
    // Documents/Introduction%20to%20atmospheric%20chemistry.pdf (page 10).
    // TODO: Add parameters to control this profile layer
    std::vector<DensityProfileLayer> ozone_density;
    ozone_density.push_back(DensityProfileLayer(25000.0, 0.0, 0.0, 1.0 / 15000.0, -2.0 / 3.0));
    ozone_density.push_back(DensityProfileLayer(0.0, 0.0, 0.0, -1.0 / 15000.0, 8.0 / 3.0));
    glm::dvec3 absorption_extinction = (static_cast<glm::dvec3>(m_cOzoneAbsorptionCoefficient) * static_cast<double>(m_cOzoneAbsorptionScale)) / 1000.0;

    glm::dvec3 ground_albedo = static_cast<glm::dvec3>(m_cGroundAlbedo);
    double bottom_radius = static_cast<double>(m_cPlanetRadius) * 1000.0;
    double top_radius = bottom_radius + static_cast<double>(m_cAtmosphereHeight) * 1000.0;
    double mie_phase_function_g = static_cast<double>(m_cMiePhaseFunctionG);
    double sun_angular_radius = static_cast<double>(m_cSunAngularRadius);

    glm::dvec3 moon_irradiance = sun_irradiance;
    double moon_angular_radius = sun_angular_radius;

    m_solarModel.reset(new Model(sun_irradiance, sun_angular_radius, moon_irradiance, moon_angular_radius,
        bottom_radius, top_radius, { rayleigh_layer }, rayleigh_scattering,
        { mie_layer }, mie_scattering, mie_extinction, mie_phase_function_g,
        ozone_density, absorption_extinction, ground_albedo, max_sun_zenith_angle,
        kLengthUnitInMeters, use_combined_textures_, use_half_precision_, SOURCE_SUN));
    m_solarModel->Init();

    m_lunarModel.reset(new Model(sun_irradiance, sun_angular_radius, moon_irradiance, moon_angular_radius,
        bottom_radius, top_radius, { rayleigh_layer }, rayleigh_scattering,
        { mie_layer }, mie_scattering, mie_extinction, mie_phase_function_g,
        ozone_density, absorption_extinction, ground_albedo, max_sun_zenith_angle,
        kLengthUnitInMeters, use_combined_textures_, use_half_precision_, SOURCE_MOON));
    m_lunarModel->Init();


    glViewport(viewportData[0], viewportData[1], viewportData[2], viewportData[3]);

    if (glGetError() != GL_NO_ERROR) std::cerr << "[OpenGL] E: Initializing model." << std::endl;

    /*
    <p>Then, it creates and compiles the vertex and fragment shaders used to render
    our demo scene, and link them with the <code>Model</code>'s atmosphere shader
    to get the final scene rendering program:
    */

    /*
    <p>Finally, it sets the uniforms of this program that can be set once and for
    all (in our case this includes the <code>Model</code>'s texture uniforms,
    because our demo app does not have any texture of its own):
    */
    // SetRenderingContext();
    InitShaders();
}


void PhysicalSky::InitShaders()
{
    ShaderStage skyVertexShader = ShaderStage();
    skyVertexShader.Create(ShaderType::VERTEX);
    skyVertexShader.Compile("D:/dev/miri-tfm/resources/shaders/sky.vert", "D:/dev/miri-tfm/resources/shaders/");
    /*ShaderStage skyFragmentShader = ShaderStage();
    skyFragmentShader.Create(ShaderType::FRAGMENT);
    skyFragmentShader.Compile("D:/dev/miri-tfm/resources/shaders/sky.frag", "D:/dev/miri-tfm/resources/shaders/");
    m_skyShader.Create();
    m_skyShader.AttachShader(skyVertexShader.m_id);
    m_skyShader.AttachShader(skyFragmentShader.m_id);
    m_skyShader.AttachShader(m_sunModel->shader());
    m_skyShader.Build();*/

    ShaderStage demoFragmentShader = ShaderStage();
    demoFragmentShader.Create(ShaderType::FRAGMENT);
    demoFragmentShader.Compile("D:/dev/miri-tfm/resources/shaders/demo.frag", "D:/dev/miri-tfm/resources/shaders/");
    m_demoShader.Create();
    m_demoShader.AttachShader(skyVertexShader.m_id);
    m_demoShader.AttachShader(demoFragmentShader.m_id);
    m_demoShader.AttachShader(m_solarModel->shader());
    //m_demoShader.AttachShader(m_moonModel->shader());
    m_demoShader.Build();

    ShaderStage moonVertexShader = ShaderStage();
    moonVertexShader.Create(ShaderType::VERTEX);
    moonVertexShader.Compile("D:/dev/miri-tfm/resources/shaders/moon.vert", "D:/dev/miri-tfm/resources/shaders/");
    ShaderStage moonFragmentShader = ShaderStage();
    moonFragmentShader.Create(ShaderType::FRAGMENT);
    moonFragmentShader.Compile("D:/dev/miri-tfm/resources/shaders/moon.frag", "D:/dev/miri-tfm/resources/shaders/");
    m_moonShader.Create();
    m_moonShader.AttachShader(moonVertexShader.m_id);
    m_moonShader.AttachShader(moonFragmentShader.m_id);
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
}

void PhysicalSky::InitResources()
{
    glGenVertexArrays(1, &m_fullScreenQuadVao);
    glBindVertexArray(m_fullScreenQuadVao);
    glGenBuffers(1, &m_fullScreenQuadVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullScreenQuadVbo);
    const GLfloat vertices[] = {
        -1.0, -1.0, 1.0, 1.0, 0.0, 0.0, 1.0,
        +1.0, -1.0, 1.0, 1.0, 0.0, 0.0, 1.0,
        -1.0, +1.0, 1.0, 1.0, 0.0, 0.0, 1.0,
        +1.0, +1.0, 1.0, 1.0, 0.0, 0.0, 1.0
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
    constexpr GLuint kAttribIndex = 0;
    constexpr int kCoordsPerVertex = 4;
    glVertexAttribPointer(kAttribIndex, kCoordsPerVertex, GL_FLOAT, false, 7 * sizeof(float), 0);
    glEnableVertexAttribArray(kAttribIndex);

    glVertexAttribPointer(1, 3, GL_FLOAT, false, 7 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

}

PhysicalSky::~PhysicalSky()
{
    glDeleteBuffers(1, &m_fullScreenQuadVbo);
    glDeleteVertexArrays(1, &m_fullScreenQuadVao);
}

void PhysicalSky::Update()
{
   /* bool shouldRecomputeModel = false;
    if (ImGui::Begin("Physical Sky Settings"))
    {
        shouldRecomputeModel |= ImGui::Checkbox("Use combined textures", &use_combined_textures_);
        shouldRecomputeModel |= ImGui::Checkbox("Use half precision", &use_half_precision_);
    }
    ImGui::End();

    if (shouldRecomputeModel) InitModel();
    if (glGetError() != GL_NO_ERROR) std::cerr << "Error recomputing model" << std::endl;*/

    m_coordinates.Update();

    if (ImGui::Begin("Physical Sky Settings New"))
    {
        ImGui::Checkbox("Show Billboard", &m_showBillboard);
        /*int popStyle = 0;
        if (m_cGroundAlbedo != m_nGroundAlbedo)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
            popStyle = 1;
        }*/
        m_shouldRecomputeModel |= ImGui::ColorEdit3("Ground Albedo", glm::value_ptr(m_nGroundAlbedo), ImGuiColorEditFlags_Float);
       /* ImGui::PopStyleColor(popStyle);
        if (m_nGroundAlbedo != m_dGroundAlbedo)
        {
            ImGui::SameLine();
            if (ImGui::ArrowButton("##Ground Albedo Reset", ImGuiDir_Left))
            {
                m_nGroundAlbedo = m_dGroundAlbedo;
                m_shouldRecomputeModel = true;
            }
        }*/

        // TODO: Sun Color
        m_shouldRecomputeModel |= ImGui::SliderFloat("Sun Intensity", &m_nSunIntensity, 0.0f, 150000.0f);
        m_shouldRecomputeModel |= ImGui::SliderFloat("Sun Angular Radius", &m_nSunAngularRadius, 0.0f, 0.1f); // Change to angular diameter/size (?) This is in degrees convert to radians

        m_shouldRecomputeModel |= ImGui::SliderFloat("Planet Radius", &m_nPlanetRadius, 1.0f, 10000.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
        m_shouldRecomputeModel |= ImGui::SliderFloat("Atmosphere Height", &m_nAtmosphereHeight, 1.0f, 200.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);


        ImGui::RadioButton("None", &m_nLimbDarkeningStrategy, 0); ImGui::SameLine();
        ImGui::RadioButton("Nec96", &m_nLimbDarkeningStrategy, 1); ImGui::SameLine();
        ImGui::RadioButton("HM98", &m_nLimbDarkeningStrategy, 2); ImGui::SameLine();
        ImGui::RadioButton("Other", &m_nLimbDarkeningStrategy, 3); ImGui::SameLine();
        ImGui::RadioButton("Invalid", &m_nLimbDarkeningStrategy, 4);

        if (ImGui::CollapsingHeader("Rayleigh"))
        {
            ImGui::PushID("Rayleigh");
            m_shouldRecomputeModel |= ImGui::ColorEdit3("Scattering Coefficient", glm::value_ptr(m_nRayleighScatteringCoefficient), ImGuiColorEditFlags_Float);
            m_shouldRecomputeModel |= ImGui::SliderFloat("Scattering Scale", &m_nRayleighScatteringScale, 0.0f, 10.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            m_shouldRecomputeModel |= ImGui::SliderFloat("Exponential Distribution", &m_nRayleighExponentialDistribution, 0.5f, 20.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("Mie"))
        {
            ImGui::PushID("Mie");
            m_shouldRecomputeModel |= ImGui::ColorEdit3("Scattering Coefficient", glm::value_ptr(m_nMieScatteringCoefficient), ImGuiColorEditFlags_Float);
            m_shouldRecomputeModel |= ImGui::SliderFloat("Scattering Scale", &m_nMieScatteringScale, 0.0f, 10.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            m_shouldRecomputeModel |= ImGui::ColorEdit3("Absorption Coefficient", glm::value_ptr(m_nMieAbsorptionCoefficient), ImGuiColorEditFlags_Float);
            m_shouldRecomputeModel |= ImGui::SliderFloat("Absorption Scale", &m_nMieAbsorptionScale, 0.0f, 10.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            m_shouldRecomputeModel |= ImGui::SliderFloat("Phase Function G", &m_nMiePhaseFunctionG, 0.0f, 1.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            m_shouldRecomputeModel |= ImGui::SliderFloat("Exponential Distribution", &m_nMieExponentialDistribution, 0.5f, 20.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("Ozone"))
        {
            ImGui::PushID("Ozone");
            // TODO: Layer
            m_shouldRecomputeModel |= ImGui::ColorEdit3("Absorption Coefficient", glm::value_ptr(m_nOzoneAbsorptionCoefficient), ImGuiColorEditFlags_Float);
            m_shouldRecomputeModel |= ImGui::SliderFloat("Absorption Scale", &m_nOzoneAbsorptionScale, 0.0f, 10.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::PopID();
        }

        if (m_shouldRecomputeModel)
        {
            if (ImGui::Button("Recompute Model"))
            {
                InitModel();
                m_shouldRecomputeModel = false;
            }
        }
    }
    ImGui::End();
}

void PhysicalSky::Render(const Camera& camera, const glm::vec2& sunAngles)
{
    m_skyShader.Use();
    m_solarModel->SetProgramUniforms(m_skyShader.m_id, 0, 1, 2, 3);
    m_skyShader.SetVec3("earth_center", glm::vec3(0.0f, 0.0f, -m_cPlanetRadius));
    m_skyShader.SetVec3("sun_size", glm::vec3(glm::tan(m_cSunAngularRadius), glm::cos(m_cSunAngularRadius), 0.0f));
    glm::mat4 view_from_clip = camera.GetViewFromClipMatrix();
    m_skyShader.SetMat4("view_from_clip", view_from_clip);

    glm::mat4 permutation = glm::mat4(glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0), glm::vec4(1, 0, 0, 0), glm::vec4(0, 0, 0, 1));
    glm::vec4 cameraRight = permutation * glm::vec4(camera.GetRight(), 0.0f);
    glm::vec4 cameraForward = permutation * glm::vec4(camera.GetForward(), 0.0f);
    glm::vec4 cameraUp = permutation * glm::vec4(camera.GetUp(), 0.0f);
    glm::vec4 cameraPosition = permutation * glm::vec4(camera.GetPosition(), 1.0f);

    glm::mat4 model_from_view = glm::mat4(cameraRight, cameraUp, -cameraForward, cameraPosition);

    m_skyShader.SetVec3("camera", cameraPosition);
    m_skyShader.SetMat4("model_from_view", model_from_view);

    glm::vec2 sunCos = glm::cos(sunAngles);
    glm::vec2 sunSin = glm::sin(sunAngles);
    glm::vec3 sunDirection = glm::vec3(sunCos.x * sunSin.y, sunSin.x * sunSin.y, sunCos.y);
    m_skyShader.SetVec3("sun_direction", sunDirection);

    GLint previousDepthFunc;
    glGetIntegerv(GL_DEPTH_FUNC, &previousDepthFunc);
    glDepthFunc(GL_LEQUAL);
    {
        glBindVertexArray(m_fullScreenQuadVao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    glDepthFunc(previousDepthFunc);
}

void PhysicalSky::RenderDemo(const Camera& camera, const glm::vec2& sunAngles)
{
    m_demoShader.Use();
    m_solarModel->SetProgramUniforms(m_demoShader.m_id, 0, 1, 2, 3);
    m_lunarModel->SetProgramUniforms(m_demoShader.m_id, 4, 5, 6, 7);
    m_demoShader.SetVec3("earth_center", glm::vec3(0.0f, 0.0f, -m_cPlanetRadius));
    m_demoShader.SetVec3("sun_size", glm::vec3(glm::tan(m_cSunAngularRadius), glm::cos(m_cSunAngularRadius), 0.0f));
    glm::mat4 view_from_clip = camera.GetViewFromClipMatrix();
    m_demoShader.SetMat4("view_from_clip", view_from_clip);

    glm::mat4 permutation = glm::mat4(glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0), glm::vec4(1, 0, 0, 0), glm::vec4(0, 0, 0, 1));
    glm::vec4 cameraRight = permutation * glm::vec4(camera.GetRight(), 0.0f);
    glm::vec4 cameraForward = permutation * glm::vec4(camera.GetForward(), 0.0f);
    glm::vec4 cameraUp = permutation * glm::vec4(camera.GetUp(), 0.0f);
    glm::vec4 cameraPosition = permutation * glm::vec4(camera.GetPosition(), 1.0f);

    glm::mat4 model_from_view = glm::mat4(cameraRight, cameraUp, -cameraForward, cameraPosition);


    m_demoShader.SetVec3("camera", cameraPosition);
    m_demoShader.SetMat4("model_from_view", model_from_view);

    glm::vec3 newSunAngles = m_coordinates.GetSunPosition();

    glm::vec2 sunCos = glm::cos(newSunAngles);
    glm::vec2 sunSin = glm::sin(newSunAngles);
    glm::vec3 sunDirection = glm::vec3(sunCos.y * sunCos.x, sunCos.y * sunSin.x, sunSin.y);
    
    m_demoShader.SetVec3("sun_direction", sunDirection);

    glm::vec3 newMoonAngles = m_coordinates.GetMoonPosition();

    glm::vec2 moonCos = glm::cos(newMoonAngles);
    glm::vec2 moonSin = glm::sin(newMoonAngles);
    glm::vec3 moonDirection = glm::vec3(moonCos.y * moonCos.x, moonCos.y * moonSin.x, moonSin.y);
    m_demoShader.SetVec3("moon_direction", moonDirection);

    m_demoShader.SetVec3("ground_albedo", m_cGroundAlbedo);
    m_demoShader.SetInt("limb_darkening_strategy", m_cLimbDarkeningStrategy);

    if (glGetError() != GL_NO_ERROR) std::cerr << "E: Setting uniforms" << std::endl;


    GLint previousDepthFunc;
    glGetIntegerv(GL_DEPTH_FUNC, &previousDepthFunc);
    glDepthFunc(GL_LEQUAL);
    {
        glBindVertexArray(m_fullScreenQuadVao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    glDepthFunc(previousDepthFunc);

    // NEW CODE
    if (m_showBillboard)
    {
        // RENDER SUN AS BILLBOARD
        m_sunShader.Use();
        m_solarModel->SetProgramUniforms(m_sunShader.m_id, 0, 1, 2, 3);

        //glm::mat4 horizon_to_ogl = glm::mat4(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        glm::mat4 horizon_to_ogl = glm::transpose(permutation);
        sunDirection = glm::vec3(horizon_to_ogl * glm::vec4(sunDirection, 0.0f));

        glm::vec3 position = camera.GetPosition() + sunDirection;
        glm::vec3 forward = glm::normalize(sunDirection);
        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::vec3 up = glm::cross(right, forward);
        glm::mat4 model = glm::mat4(glm::vec4(right, 0.0f), glm::vec4(up, 0.0f), glm::vec4(-forward, 0.0), glm::vec4(position, 1.0));
        //glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, camera.GetPosition() + glm::vec3(1.0f, 0.0f, -1.0f));

        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(glm::tan(m_cSunAngularRadius)));
        m_sunShader.SetMat4("model", model * scale);
        m_sunShader.SetMat4("view", camera.GetViewMatrix());
        m_sunShader.SetMat4("projection", camera.GetProjectionMatrix());

        m_sunShader.SetVec3("w_CameraPos", camera.GetPosition());
        m_sunShader.SetVec3("w_PlanetPos", glm::vec3(0.0f, -m_cPlanetRadius, 0.0f));
        m_sunShader.SetVec3("w_SunDirection", sunDirection);

        glBindVertexArray(m_fullScreenQuadVao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


        // RENDER MOON AS BILLBOARD
        m_moonShader.Use();
        m_solarModel->SetProgramUniforms(m_moonShader.m_id, 0, 1, 2, 3);

        glm::vec3 moonHorizonCoordinates = m_coordinates.GetMoonPosition();
        glm::vec2 moonHorizonCos = glm::cos(moonHorizonCoordinates);
        glm::vec2 moonHorizonSin = glm::sin(moonHorizonCoordinates);
        glm::vec3 moonHorizonDirection = glm::vec3(moonHorizonCos.y * moonHorizonCos.x, moonHorizonCos.y * moonHorizonSin.x, moonHorizonSin.y);
        glm::vec3 moonWorldDirection = glm::vec3(horizon_to_ogl * glm::vec4(moonHorizonDirection, 0.0));

        glm::vec3 moonPosition = camera.GetPosition() + moonWorldDirection;
        glm::vec3 moonForward = glm::normalize(moonWorldDirection);
        glm::vec3 moonRight = glm::normalize(glm::cross(moonForward, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::vec3 moonUp = glm::cross(moonRight, moonForward);
        glm::mat4 moonModel = glm::mat4(glm::vec4(moonRight, 0.0f), glm::vec4(moonUp, 0.0f), glm::vec4(-moonForward, 0.0), glm::vec4(moonPosition, 1.0));
        glm::mat4 moonScale = scale; // TODO: Get moon angular size correctly

        m_moonShader.SetMat4("model", moonModel * scale);
        m_moonShader.SetMat4("view", camera.GetViewMatrix());
        m_moonShader.SetMat4("projection", camera.GetProjectionMatrix());

        glBindVertexArray(m_fullScreenQuadVao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        /*m_sunShader.SetVec3("w_CameraPos", camera.GetPosition());
        m_sunShader.SetVec3("w_PlanetPos", glm::vec3(0.0f, -m_cPlanetRadius, 0.0f));
        m_sunShader.SetVec3("w_SunDirection", sunDirection);*/
    }
}