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

PhysicalSky::PhysicalSky()
    : m_groundAlbedo(0.401978f, 0.401978f, 0.401978f) // unitless
    , m_sunIntensity(100000.000000f) // lux
    , m_sunAngularRadius(0.004675f) // !
    , m_planetRadius(6360.0f) // km
    , m_atmosphereHeight(60.0f) // km
    , m_rayleighScatteringCoefficient(0.175287f, 0.409607f, 1.000000f) // unitless
    , m_rayleighScatteringScale(0.033100f) // km^-1
    , m_rayleighExponentialDistribution(8.000000f) // km
    , m_mieScatteringCoefficient(1.000000f, 1.000000f, 1.000000f) // unitless
    , m_mieScatteringScale(0.003996f) // km^-1
    , m_mieAbsorptionCoefficient(1.000000f, 1.000000f, 1.000000f) // unitless
    , m_mieAbsorptionScale(0.000444f) // km^-1
    , m_miePhaseFunctionG(0.800000f) // unitless
    , m_mieExponentialDistribution(1.200000f) // km
    , m_ozoneAbsorptionCoefficient(0.345561f, 1.000000f, 0.045189f) // unitless
    , m_ozoneAbsorptionScale(0.001881f) // km^-1
    , m_shouldRecomputeModel(false)
    , m_limbDarkeningStrategy(0)
    , use_combined_textures_(false)
    , use_half_precision_(false)
{
    Init();
}

void PhysicalSky::Init()
{
    InitModel();
    InitResources();
}

void PhysicalSky::InitModel()
{
    const double max_sun_zenith_angle =
        (use_half_precision_ ? 102.0 : 120.0) / 180.0 * kPi;

    int viewportData[4];
    glGetIntegerv(GL_VIEWPORT, viewportData);

    glm::dvec3 solar_irradiance = glm::dvec3(1.0) * static_cast<double>(m_sunIntensity);

    DensityProfileLayer rayleigh_layer(0.0, 1.0, -1.0 / (static_cast<double>(m_rayleighExponentialDistribution) * 1000.0), 0.0, 0.0);
    glm::dvec3 rayleigh_scattering = (static_cast<glm::dvec3>(m_rayleighScatteringCoefficient) * static_cast<double>(m_rayleighScatteringScale)) / 1000.0;

    DensityProfileLayer mie_layer(0.0, 1.0, -1.0 / (static_cast<double>(m_mieExponentialDistribution) * 1000.0), 0.0, 0.0);
    glm::dvec3 mie_scattering = (static_cast<glm::dvec3>(m_mieScatteringCoefficient) * static_cast<double>(m_mieScatteringScale)) / 1000.0;
    glm::dvec3 mie_extinction = mie_scattering + (static_cast<glm::dvec3>(m_mieAbsorptionCoefficient) * static_cast<double>(m_mieAbsorptionScale)) / 1000.0;

    // Density profile increasing linearly from 0 to 1 between 10 and 25km, and
    // decreasing linearly from 1 to 0 between 25 and 40km. This is an approximate
    // profile from http://www.kln.ac.lk/science/Chemistry/Teaching_Resources/
    // Documents/Introduction%20to%20atmospheric%20chemistry.pdf (page 10).
    // TODO: Add parameters to control this profile layer
    std::vector<DensityProfileLayer> ozone_density;
    ozone_density.push_back(DensityProfileLayer(25000.0, 0.0, 0.0, 1.0 / 15000.0, -2.0 / 3.0));
    ozone_density.push_back(DensityProfileLayer(0.0, 0.0, 0.0, -1.0 / 15000.0, 8.0 / 3.0));
    glm::dvec3 absorption_extinction = (static_cast<glm::dvec3>(m_ozoneAbsorptionCoefficient) * static_cast<double>(m_ozoneAbsorptionScale)) / 1000.0;

    glm::dvec3 ground_albedo = static_cast<glm::dvec3>(m_groundAlbedo);
    double bottom_radius = static_cast<double>(m_planetRadius) * 1000.0;
    double top_radius = bottom_radius + static_cast<double>(m_atmosphereHeight) * 1000.0;
    double mie_phase_function_g = static_cast<double>(m_miePhaseFunctionG);
    double sun_angular_radius = static_cast<double>(m_sunAngularRadius);

    m_model.reset(new Model(solar_irradiance, sun_angular_radius,
        bottom_radius, top_radius, { rayleigh_layer }, rayleigh_scattering,
        { mie_layer }, mie_scattering, mie_extinction, mie_phase_function_g,
        ozone_density, absorption_extinction, ground_albedo, max_sun_zenith_angle,
        kLengthUnitInMeters, use_combined_textures_, use_half_precision_));
    m_model->Init();
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

    ShaderStage skyFragmentShader = ShaderStage();
    skyFragmentShader.Create(ShaderType::FRAGMENT);
    skyFragmentShader.Compile("D:/dev/miri-tfm/resources/shaders/sky.frag", "D:/dev/miri-tfm/resources/shaders/");

    m_skyShader.Create();
    m_skyShader.AttachShader(skyVertexShader.m_id);
    m_skyShader.AttachShader(skyFragmentShader.m_id);
    m_skyShader.AttachShader(m_model->shader());
    m_skyShader.Build();

    ShaderStage demoFragmentShader = ShaderStage();
    demoFragmentShader.Create(ShaderType::FRAGMENT);
    demoFragmentShader.Compile("D:/dev/miri-tfm/resources/shaders/demo.frag", "D:/dev/miri-tfm/resources/shaders/");
    m_demoShader.Create();
    m_demoShader.AttachShader(skyVertexShader.m_id);
    m_demoShader.AttachShader(demoFragmentShader.m_id);
    m_demoShader.AttachShader(m_model->shader());
    m_demoShader.Build();
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

    bool shouldRecomputeModel = false;
    if (ImGui::Begin("Physical Sky Settings"))
    {
        shouldRecomputeModel |= ImGui::Checkbox("Use combined textures", &use_combined_textures_);
        shouldRecomputeModel |= ImGui::Checkbox("Use half precision", &use_half_precision_);

        
    }
    ImGui::End();

    if (shouldRecomputeModel) InitModel();
    if (glGetError() != GL_NO_ERROR) std::cerr << "Error recomputing model" << std::endl;


    if (ImGui::Begin("Physical Sky Settings New"))
    {
        m_shouldRecomputeModel |= ImGui::ColorEdit3("Ground Albedo", glm::value_ptr(m_groundAlbedo), ImGuiColorEditFlags_Float);

        // TODO: Sun Color
        m_shouldRecomputeModel |= ImGui::SliderFloat("Sun Intensity", &m_sunIntensity, 0.0f, 150000.0f);
        m_shouldRecomputeModel |= ImGui::SliderFloat("Sun Angular Radius", &m_sunAngularRadius, 0.0f, 0.1f); // Change to angular diameter/size (?) This is in degrees convert to radians

        m_shouldRecomputeModel |= ImGui::SliderFloat("Planet Radius", &m_planetRadius, 1.0f, 10000.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
        m_shouldRecomputeModel |= ImGui::SliderFloat("Atmosphere Height", &m_atmosphereHeight, 1.0f, 200.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);


        ImGui::RadioButton("None", &m_limbDarkeningStrategy, 0); ImGui::SameLine();
        ImGui::RadioButton("Nec96", &m_limbDarkeningStrategy, 1); ImGui::SameLine();
        ImGui::RadioButton("HM98", &m_limbDarkeningStrategy, 2); ImGui::SameLine();
        ImGui::RadioButton("Other", &m_limbDarkeningStrategy, 3); ImGui::SameLine();
        ImGui::RadioButton("Invalid", &m_limbDarkeningStrategy, 4);

        if (ImGui::CollapsingHeader("Rayleigh"))
        {
            ImGui::PushID("Rayleigh");
            m_shouldRecomputeModel |= ImGui::ColorEdit3("Scattering Coefficient", glm::value_ptr(m_rayleighScatteringCoefficient), ImGuiColorEditFlags_Float);
            m_shouldRecomputeModel |= ImGui::SliderFloat("Scattering Scale", &m_rayleighScatteringScale, 0.0f, 10.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            m_shouldRecomputeModel |= ImGui::SliderFloat("Exponential Distribution", &m_rayleighExponentialDistribution, 0.5f, 20.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("Mie"))
        {
            ImGui::PushID("Mie");
            m_shouldRecomputeModel |= ImGui::ColorEdit3("Scattering Coefficient", glm::value_ptr(m_mieScatteringCoefficient), ImGuiColorEditFlags_Float);
            m_shouldRecomputeModel |= ImGui::SliderFloat("Scattering Scale", &m_mieScatteringScale, 0.0f, 10.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            m_shouldRecomputeModel |= ImGui::ColorEdit3("Absorption Coefficient", glm::value_ptr(m_mieAbsorptionCoefficient), ImGuiColorEditFlags_Float);
            m_shouldRecomputeModel |= ImGui::SliderFloat("Absorption Scale", &m_mieAbsorptionScale, 0.0f, 10.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            m_shouldRecomputeModel |= ImGui::SliderFloat("Phase Function G", &m_miePhaseFunctionG, 0.0f, 1.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            m_shouldRecomputeModel |= ImGui::SliderFloat("Exponential Distribution", &m_mieExponentialDistribution, 0.5f, 20.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("Ozone"))
        {
            ImGui::PushID("Ozone");
            // TODO: Layer
            m_shouldRecomputeModel |= ImGui::ColorEdit3("Absorption Coefficient", glm::value_ptr(m_ozoneAbsorptionCoefficient), ImGuiColorEditFlags_Float);
            m_shouldRecomputeModel |= ImGui::SliderFloat("Absorption Scale", &m_ozoneAbsorptionScale, 0.0f, 10.0f, "%.6f", ImGuiSliderFlags_AlwaysClamp);
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
    m_model->SetProgramUniforms(m_skyShader.m_id, 0, 1, 2, 3);
    m_skyShader.SetVec3("earth_center", glm::vec3(0.0f, 0.0f, -m_planetRadius));
    m_skyShader.SetVec3("sun_size", glm::vec3(glm::tan(m_sunAngularRadius), glm::cos(m_sunAngularRadius), 0.0f));
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
    m_model->SetProgramUniforms(m_demoShader.m_id, 0, 1, 2, 3);
    m_demoShader.SetVec3("earth_center", glm::vec3(0.0f, 0.0f, -m_planetRadius));
    m_demoShader.SetVec3("sun_size", glm::vec3(glm::tan(m_sunAngularRadius), glm::cos(m_sunAngularRadius), 0.0f));
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

    glm::vec2 sunCos = glm::cos(sunAngles);
    glm::vec2 sunSin = glm::sin(sunAngles);
    glm::vec3 sunDirection = glm::vec3(sunCos.x * sunSin.y, sunSin.x * sunSin.y, sunCos.y);
    m_demoShader.SetVec3("sun_direction", sunDirection);
    m_demoShader.SetVec3("ground_albedo", m_groundAlbedo);
    m_demoShader.SetInt("limb_darkening_strategy", m_limbDarkeningStrategy);

    if (glGetError() != GL_NO_ERROR) std::cerr << "E: Setting uniforms" << std::endl;


    GLint previousDepthFunc;
    glGetIntegerv(GL_DEPTH_FUNC, &previousDepthFunc);
    glDepthFunc(GL_LEQUAL);
    {
        glBindVertexArray(m_fullScreenQuadVao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    glDepthFunc(previousDepthFunc);
}