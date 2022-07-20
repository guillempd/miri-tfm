/**
 * Copyright (c) 2017 Eric Bruneton
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/*<h2>atmosphere/demo/demo.cc</h2>

<p>This file, together with the shader <a href="demo.glsl.html">demo.glsl</a>,
shows how the API provided in <a href="../model.h.html">model.h</a> can be used
in practice. It implements the <code>Demo</code> class whose header is defined
in <a href="demo.h.html">demo.h</a> (note that most of the following code is
independent of our atmosphere model. The only part which is related to it is the
<code>InitModel</code> method).
*/

#include "PhysicalSky.h"

#include "Window.h"

#include <imgui.h>

#include <glad/glad.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

/*
<p>Our demo application renders a simple scene made of a purely spherical planet
and a large sphere (of 1km radius) lying on it. This scene is displayed by
rendering a full screen quad, with a fragment shader computing the color of each
pixel by "ray tracing" (which is very simple here because the scene consists of
only two spheres). The vertex shader is thus very simple, and is provided in the
following constant. The fragment shader is more complex, and is defined in the
separate file <a href="demo.glsl.html">demo.glsl</a> (which is included here as
a string literal via the generated file <code>demo.glsl.inc</code>):
*/

namespace {

    constexpr double kPi = 3.1415926;
    //constexpr double kSunAngularRadius = 0.00935 / 2.0;
    //constexpr double kSunSolidAngle = kPi * kSunAngularRadius * kSunAngularRadius;
    constexpr double kLengthUnitInMeters = 1000.0;

    const char kVertexShader[] = R"(
    #version 330
    uniform mat4 model_from_view;
    uniform mat4 view_from_clip;
    layout(location = 0) in vec4 vertex;
    out vec3 view_ray;
    void main() {
      view_ray =
          (model_from_view * vec4((view_from_clip * vec4(vertex.xy, 1.0, 0.0)).xyz, 0.0)).xyz;
      gl_Position = vertex;
    })";

#include <atmosphere/demo/demo.glsl.inc>

}  // anonymous namespace

/*
<p>The class constructor is straightforward and completely independent of our
atmosphere model (which is initialized in the separate method
<code>InitModel</code>). It's main role is to create the demo window, to set up
the event handler callbacks (it does so in such a way that several Demo
instances can be created at the same time, using the <code>INSTANCES</code>
global variable), and to create the vertex buffers and the text renderer that
will be used to render the scene and the help messages:
*/

using namespace atmosphere;

PhysicalSky::PhysicalSky()
    : use_combined_textures_(false)
    , use_half_precision_(false)
    , do_white_balance_(false)
    , program_(0)
    , sun_zenith_angle_radians_(1.3)
    , sun_azimuth_angle_radians_(2.9)
    , exposure_(5e-5)
    , m_groundAlbedo                    (0.401978f, 0.401978f, 0.401978f) // unitless
    , m_sunIntensity                    (100000.000000f) // lux
    , m_sunAngularRadius                (0.004675f) // !
    , m_planetRadius                    (6360.0f) // km
    , m_atmosphereHeight                (60.0f) // km
    , m_rayleighScatteringCoefficient   (0.175287f, 0.409607f, 1.000000f) // unitless
    , m_rayleighScatteringScale         (0.033100f) // km^-1
    , m_rayleighExponentialDistribution (8.000000f) // km
    , m_mieScatteringCoefficient        (1.000000f, 1.000000f, 1.000000f) // unitless
    , m_mieScatteringScale              (0.003996f) // km^-1
    , m_mieAbsorptionCoefficient        (1.000000f, 1.000000f, 1.000000f) // unitless
    , m_mieAbsorptionScale              (0.000444f) // km^-1
    , m_miePhaseFunctionG               (0.800000f) // unitless
    , m_mieExponentialDistribution      (1.200000f) // km
    , m_ozoneAbsorptionCoefficient      (0.345561f, 1.000000f, 0.045189f) // unitless
    , m_ozoneAbsorptionScale            (0.001881f) // km^-1
    , m_shouldRecomputeModel            (false)
    , m_mesh()
    , m_meshShader()
{
    m_mesh = std::make_unique<Mesh>("D:/Escritorio/sphere.gltf");
}

/*
<p>The destructor is even simpler:
*/

PhysicalSky::~PhysicalSky() {
    glDeleteShader(vertex_shader_);
    glDeleteShader(fragment_shader_);
    glDeleteProgram(program_);
    glDeleteBuffers(1, &full_screen_quad_vbo_);
    glDeleteVertexArrays(1, &full_screen_quad_vao_);
}

void PhysicalSky::Init(Window* window) {
    is_mouse_button_pressed_ = false;
    InitResources();
    InitModel();
    ShaderStage vertexShader = ShaderStage(ShaderType::VERTEX);
    ShaderStage fragmentShader = ShaderStage(ShaderType::FRAGMENT);
    vertexShader.Compile("D:/dev/miri-tfm/resources/shaders/meshNew.vert", "D:/dev/miri-tfm/resources/shaders/");
    fragmentShader.Compile("D:/dev/miri-tfm/resources/shaders/meshNew.frag", "D:/dev/miri-tfm/resources/shaders/");
    m_meshShader.AttachShader(vertexShader.m_id);
    m_meshShader.AttachShader(fragmentShader.m_id);
    m_meshShader.AttachShader(model_->shader());
    m_meshShader.Build();
}

void PhysicalSky::InitResources() {
    glGenVertexArrays(1, &full_screen_quad_vao_);
    glBindVertexArray(full_screen_quad_vao_);
    glGenBuffers(1, &full_screen_quad_vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, full_screen_quad_vbo_);
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

/*
<p>The "real" initialization work, which is specific to our atmosphere model,
is done in the following method. It starts with the creation of an atmosphere
<code>Model</code> instance, with parameters corresponding to the Earth
atmosphere:
*/

void PhysicalSky::InitModel() {
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

    model_.reset(new Model(solar_irradiance, sun_angular_radius,
        bottom_radius, top_radius, { rayleigh_layer }, rayleigh_scattering,
        { mie_layer }, mie_scattering, mie_extinction, mie_phase_function_g,
        ozone_density, absorption_extinction, ground_albedo, max_sun_zenith_angle,
        kLengthUnitInMeters, use_combined_textures_, use_half_precision_));
    model_->Init();
    glViewport(viewportData[0], viewportData[1], viewportData[2], viewportData[3]);

    if (glGetError() != GL_NO_ERROR) std::cerr << "[OpenGL] E: Initializing model." << std::endl;

    /*
    <p>Then, it creates and compiles the vertex and fragment shaders used to render
    our demo scene, and link them with the <code>Model</code>'s atmosphere shader
    to get the final scene rendering program:
    */

    vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
    const char* const vertex_shader_source = kVertexShader;
    std::cout << "----------------------VERTEX SHADER SOURCE----------------------" << std::endl;
    std::cout << vertex_shader_source << std::endl;
    std::cout << "------------------------------------------------------------------" << std::endl;
    glShaderSource(vertex_shader_, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader_);

    GLint success;
    glGetShaderiv(vertex_shader_, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[4096];
        glGetShaderInfoLog(vertex_shader_, 4096, nullptr, infoLog);
        std::cerr << infoLog << std::endl;
    }

    const std::string fragment_shader_str =
        "#version 330\n"
        "const float kLengthUnitInMeters = " +
        std::to_string(kLengthUnitInMeters) + ";\n" +
        demo_glsl;
    const char* fragment_shader_source = fragment_shader_str.c_str();
    std::cout << "----------------------FRAGMENT SHADER SOURCE----------------------" << std::endl;
    std::cout << fragment_shader_source << std::endl;
    std::cout << "------------------------------------------------------------------" << std::endl;
    fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader_);

    glGetShaderiv(fragment_shader_, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[4096];
        glGetShaderInfoLog(fragment_shader_, 4096, nullptr, infoLog);
        std::cerr << infoLog << std::endl;
    }

    if (program_ != 0) {
        glDeleteProgram(program_);
    }
    program_ = glCreateProgram();
    glAttachShader(program_, vertex_shader_);
    glAttachShader(program_, fragment_shader_);
    glAttachShader(program_, model_->shader());
    glLinkProgram(program_);
    glDetachShader(program_, vertex_shader_);
    glDetachShader(program_, fragment_shader_);
    glDetachShader(program_, model_->shader());

    glGetProgramiv(program_, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[4096];
        glGetProgramInfoLog(program_, 4096, nullptr, infoLog);
        std::cerr << infoLog << std::endl;
    }

    if (glGetError() != GL_NO_ERROR) std::cerr << "[OpenGL] E: Compiling atmosphere shaders." << std::endl;

    // NOTE(guillem): Code added by me
    // Init mesh shader
    /*m_meshProgram.reset(new ShaderProgram());
    ShaderStage vertexShaderSource = ShaderStage("D:/dev/miri-tfm/resources/shaders/meshPhysical.vert");
    ShaderStage fragmentShaderSource = ShaderStage("D:/dev/miri-tfm/resources/shaders/meshPhysical.frag");
    if (use_luminance_ != Luminance::NONE) fragmentShaderSource.AddDefine("USE_LUMINANCE");
    m_meshProgram->AttachShader(model_->shader());
    m_meshProgram->Build(vertexShaderSource, fragmentShaderSource);*/

    if (glGetError() != GL_NO_ERROR) std::cerr << "[OpenGL] E: Compiling mesh shader." << std::endl;

    /*
    <p>Finally, it sets the uniforms of this program that can be set once and for
    all (in our case this includes the <code>Model</code>'s texture uniforms,
    because our demo app does not have any texture of its own):
    */
    // SetRenderingContext();
}

void PhysicalSky::SetRenderingContext(const Camera& camera) const {
    glUseProgram(program_);
    model_->SetProgramUniforms(program_, 0, 1, 2, 3);
    if (glGetError() != GL_NO_ERROR) std::cerr << "[OpenGL] E: After setting program uniforms." << std::endl;

    double white_point_r = 1.0;
    double white_point_g = 1.0;
    double white_point_b = 1.0;
    // TODO: Investigate about this white balance
    /*if (do_white_balance_) {
        Model::ConvertSpectrumToLinearSrgb(m_wavelengths, m_solar_irradiance,
            &white_point_r, &white_point_g, &white_point_b);
        double white_point = (white_point_r + white_point_g + white_point_b) / 3.0;
        white_point_r /= white_point;
        white_point_g /= white_point;
        white_point_b /= white_point;
    }*/
    glUniform3f(glGetUniformLocation(program_, "white_point"), white_point_r, white_point_g, white_point_b);
    glUniform3f(glGetUniformLocation(program_, "earth_center"), 0.0, 0.0, -m_planetRadius);
    glUniform2f(glGetUniformLocation(program_, "sun_size"), tan(m_sunAngularRadius), cos(m_sunAngularRadius));
    glUniform3f(glGetUniformLocation(program_, "ground_albedo"), m_groundAlbedo.r, m_groundAlbedo.g, m_groundAlbedo.b);

    glm::mat4 view_from_clip = camera.GetViewFromClipMatrix();
    glUniformMatrix4fv(glGetUniformLocation(program_, "view_from_clip"), 1, GL_FALSE, glm::value_ptr(view_from_clip));
}

/*
<p>The scene rendering method simply sets the uniforms related to the camera
position and to the Sun direction, and then draws a full screen quad (and
optionally a help screen).
*/

void PhysicalSky::Render(const Camera& camera) {
    RenderUi();
    if (glGetError() != GL_NO_ERROR) std::cerr << "[OpenGL] E: After rendering UI." << std::endl;
    SetRenderingContext(camera);
    if (glGetError() != GL_NO_ERROR) std::cerr << "[OpenGL] E: After setting context." << std::endl;

    // NOTE: Transform from our camera approach (classical opengl) to their camera approach (mathematical approach)
    // Might be ok to move these to the camera class
    // IDEA: Create a compass widget to know in which direction we are looking at

     if (glGetError() != GL_NO_ERROR) std::cerr << "[OpenGL] E: Pre submitting uniforms." << std::endl;

    glm::mat4 permutation = glm::mat4(glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0), glm::vec4(1, 0, 0, 0), glm::vec4(0, 0, 0, 1));
    glm::vec4 cameraRight = permutation * glm::vec4(camera.GetRight(), 0.0f);
    glm::vec4 cameraForward = permutation * glm::vec4(camera.GetForward(), 0.0f);
    glm::vec4 cameraUp = permutation * glm::vec4(camera.GetUp(), 0.0f);
    glm::vec4 cameraPosition = permutation * glm::vec4(camera.GetPosition(), 1.0f);

    glm::mat4 model_from_view_ = glm::mat4(cameraRight, cameraUp, -cameraForward, cameraPosition);
    glUniform3fv(glGetUniformLocation(program_, "camera"), 1, glm::value_ptr(cameraPosition));
    glUniformMatrix4fv(glGetUniformLocation(program_, "model_from_view"), 1, GL_FALSE, glm::value_ptr(model_from_view_));
    glUniform1f(glGetUniformLocation(program_, "exposure"), exposure_);

    glm::vec2 sunAngles = glm::vec2(sun_azimuth_angle_radians_, sun_zenith_angle_radians_);
    glm::vec2 sunCos = glm::cos(sunAngles);
    glm::vec2 sunSin = glm::sin(sunAngles);
    glm::vec3 sunDirection = glm::vec3(sunCos.x * sunSin.y, sunSin.x * sunSin.y, sunCos.y);
    glUniform3fv(glGetUniformLocation(program_, "sun_direction"), 1, glm::value_ptr(sunDirection));
    if (glGetError() != GL_NO_ERROR) std::cerr << "[OpenGL] E: Submitting uniforms." << std::endl;

    GLint previousDepthFunc;
    glGetIntegerv(GL_DEPTH_FUNC, &previousDepthFunc);
    glDepthFunc(GL_LEQUAL);
    {
        //glBindVertexArray(full_screen_quad_vao_);
        //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        m_meshShader.Use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.0f));
        m_meshShader.SetMat4("model", model);
        m_meshShader.SetMat4("view", camera.GetViewMatrix());
        m_meshShader.SetMat4("projection", camera.GetProjectionMatrix());
        m_meshShader.SetVec3("w_LightDir", glm::vec3(sunSin.y * sunSin.x, sunCos.y, sunSin.y * sunCos.x));
        m_meshShader.SetVec3("w_CameraPos", camera.GetPosition());
        m_meshShader.SetVec3("w_PlanetPos", glm::vec3(0.0f, -m_planetRadius, 0.0f));
        model_->SetProgramUniforms(m_meshShader.m_id, 0, 1, 2, 3);

        m_mesh->JustRender(camera);

    //    m_meshProgram->Use();
    //    m_meshProgram->SetMat4("model", glm::mat4(1.0f));
    //    m_meshProgram->SetMat4("view", camera.GetViewMatrix());
    //    m_meshProgram->SetMat4("projection", camera.GetProjectionMatrix());
    //    m_meshProgram->SetVec3("w_CameraPos", camera.GetPosition());
    //    m_meshProgram->SetVec3("w_EarthCenterPos", glm::vec3(0.0f, -m_BottomRadius / kLengthUnitInMeters, 0.0f));
    //    m_meshProgram->SetVec3("w_SunDirection", glm::vec3(sunSin.y * sunSin.x, sunCos.y, sunSin.y * sunCos.x));

    //    // NOTE: Review these two following values
    //    m_meshProgram->SetFloat("exposure", use_luminance_ != Luminance::NONE ? exposure_ * 1e-5 : exposure_);
    //    m_meshProgram->SetVec3("white_point", glm::vec3(1.0));
    //    // TODO: Draw another mesh
    //    // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    glDepthFunc(previousDepthFunc);

}

/*
<p>The other event handling methods are also straightforward, and do not
interact with the atmosphere model:
*/

void PhysicalSky::RenderUi()
{
    bool shouldRecomputeModel = false;
    if (ImGui::Begin("Physical Sky Settings"))
    {
        shouldRecomputeModel |= ImGui::Checkbox("Use combined textures", &use_combined_textures_);
        shouldRecomputeModel |= ImGui::Checkbox("Use half precision", &use_half_precision_);

        ImGui::Checkbox("Do white balance", &do_white_balance_);

        float exposure_f = exposure_;
        ImGui::DragFloat("Exposure", &exposure_f, 0.001f, 1e-5, 1e2, "%.6f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);
        exposure_ = exposure_f;

        ImGui::Text("Predefined Views");
        if (ImGui::Button("1")) SetView(9000.0, 1.47, 0.0, 1.3, 3.0, 10.0);
        ImGui::SameLine();
        if (ImGui::Button("2")) SetView(9000.0, 1.47, 0.0, 1.564, -3.0, 10.0);
        ImGui::SameLine();
        if (ImGui::Button("3")) SetView(7000.0, 1.57, 0.0, 1.54, -2.96, 10.0);
        ImGui::SameLine();
        if (ImGui::Button("4")) SetView(7000.0, 1.57, 0.0, 1.328, -3.044, 10.0);
        ImGui::SameLine();
        if (ImGui::Button("5")) SetView(9000.0, 1.39, 0.0, 1.2, 0.7, 10.0);
        ImGui::SameLine();
        if (ImGui::Button("6")) SetView(9000.0, 1.5, 0.0, 1.628, 1.05, 200.0);
        ImGui::SameLine();
        if (ImGui::Button("7")) SetView(7000.0, 1.43, 0.0, 1.57, 1.34, 40.0);
        ImGui::SameLine();
        if (ImGui::Button("8")) SetView(2.7e6, 0.81, 0.0, 1.57, 2.0, 10.0);
        ImGui::SameLine();
        if (ImGui::Button("9")) SetView(1.2e7, 0.0, 0.0, 0.93, -2.0, 10.0);
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

void PhysicalSky::OnMouseClick(int button, int action, int mods) {
    is_mouse_button_pressed_ = (action == GLFW_PRESS);
    is_ctrl_key_pressed_ = (mods & GLFW_MOD_CONTROL);
}

bool PhysicalSky::OnCursorMovement(glm::vec2 movement) {
    if (!is_mouse_button_pressed_) return false;

    constexpr double kScale = 500.0;
    if (is_ctrl_key_pressed_)
    {
        sun_zenith_angle_radians_ -= movement.y / kScale;
        sun_zenith_angle_radians_ = std::max(0.0, std::min(kPi, sun_zenith_angle_radians_));
        sun_azimuth_angle_radians_ += movement.x / kScale;
        return true;
    }
    return false;
}

void PhysicalSky::SetView(double view_distance_meters,
    double view_zenith_angle_radians, double view_azimuth_angle_radians,
    double sun_zenith_angle_radians, double sun_azimuth_angle_radians,
    double exposure) {
    // NOTE: This should set the camera settings
    //view_distance_meters_ = view_distance_meters;
    // view_zenith_angle_radians_ = view_zenith_angle_radians;
    // view_azimuth_angle_radians_ = view_azimuth_angle_radians;
    sun_zenith_angle_radians_ = sun_zenith_angle_radians;
    sun_azimuth_angle_radians_ = sun_azimuth_angle_radians;
    exposure_ = exposure;
}
