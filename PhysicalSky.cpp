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
    constexpr double kSunAngularRadius = 0.00935 / 2.0;
    constexpr double kSunSolidAngle = kPi * kSunAngularRadius * kSunAngularRadius;
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

PhysicalSky::PhysicalSky(int viewport_width, int viewport_height) :
    use_constant_solar_spectrum_(false),
    use_ozone_(true),
    use_combined_textures_(true),
    use_half_precision_(true),
    use_luminance_(Luminance::NONE),
    do_white_balance_(false),
    show_help_(true),
    program_(0),
    sun_zenith_angle_radians_(1.3),
    sun_azimuth_angle_radians_(2.9),
    exposure_(10.0) {

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

void PhysicalSky::Initialize(Window* window) {
    is_mouse_button_pressed_ = false;
    InitResources();
    InitModel();
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
    // Values from "Reference Solar Spectral Irradiance: ASTM G-173", ETR column
    // (see http://rredc.nrel.gov/solar/spectra/am1.5/ASTMG173/ASTMG173.html),
    // summed and averaged in each bin (e.g. the value for 360nm is the average
    // of the ASTM G-173 values for all wavelengths between 360 and 370nm).
    // Values in W.m^-2.
    constexpr int kLambdaMin = 360;
    constexpr int kLambdaMax = 830;
    constexpr double kSolarIrradiance[48] = {
      1.11776, 1.14259, 1.01249, 1.14716, 1.72765, 1.73054, 1.6887, 1.61253,
      1.91198, 2.03474, 2.02042, 2.02212, 1.93377, 1.95809, 1.91686, 1.8298,
      1.8685, 1.8931, 1.85149, 1.8504, 1.8341, 1.8345, 1.8147, 1.78158, 1.7533,
      1.6965, 1.68194, 1.64654, 1.6048, 1.52143, 1.55622, 1.5113, 1.474, 1.4482,
      1.41018, 1.36775, 1.34188, 1.31429, 1.28303, 1.26758, 1.2367, 1.2082,
      1.18737, 1.14683, 1.12362, 1.1058, 1.07124, 1.04992
    };
    // Values from http://www.iup.uni-bremen.de/gruppen/molspec/databases/
    // referencespectra/o3spectra2011/index.html for 233K, summed and averaged in
    // each bin (e.g. the value for 360nm is the average of the original values
    // for all wavelengths between 360 and 370nm). Values in m^2.
    constexpr double kOzoneCrossSection[48] = {
      1.18e-27, 2.182e-28, 2.818e-28, 6.636e-28, 1.527e-27, 2.763e-27, 5.52e-27,
      8.451e-27, 1.582e-26, 2.316e-26, 3.669e-26, 4.924e-26, 7.752e-26, 9.016e-26,
      1.48e-25, 1.602e-25, 2.139e-25, 2.755e-25, 3.091e-25, 3.5e-25, 4.266e-25,
      4.672e-25, 4.398e-25, 4.701e-25, 5.019e-25, 4.305e-25, 3.74e-25, 3.215e-25,
      2.662e-25, 2.238e-25, 1.852e-25, 1.473e-25, 1.209e-25, 9.423e-26, 7.455e-26,
      6.566e-26, 5.105e-26, 4.15e-26, 4.228e-26, 3.237e-26, 2.451e-26, 2.801e-26,
      2.534e-26, 1.624e-26, 1.465e-26, 2.078e-26, 1.383e-26, 7.105e-27
    };
    // From https://en.wikipedia.org/wiki/Dobson_unit, in molecules.m^-2.
    constexpr double kDobsonUnit = 2.687e20;
    // Maximum number density of ozone molecules, in m^-3 (computed so at to get
    // 300 Dobson units of ozone - for this we divide 300 DU by the integral of
    // the ozone density profile defined below, which is equal to 15km).
    constexpr double kMaxOzoneNumberDensity = 300.0 * kDobsonUnit / 15000.0;
    // Wavelength independent solar irradiance "spectrum" (not physically
    // realistic, but was used in the original implementation).
    constexpr double kConstantSolarIrradiance = 1.5;
    constexpr double kTopRadius = 6420000.0;
    constexpr double kRayleigh = 1.24062e-6;
    constexpr double kRayleighScaleHeight = 8000.0;
    constexpr double kMieScaleHeight = 1200.0;
    constexpr double kMieAngstromAlpha = 0.0;
    constexpr double kMieAngstromBeta = 5.328e-3;
    constexpr double kMieSingleScatteringAlbedo = 0.9;
    constexpr double kMiePhaseFunctionG = 0.8;
    constexpr double kGroundAlbedo = 0.1;
    const double max_sun_zenith_angle =
        (use_half_precision_ ? 102.0 : 120.0) / 180.0 * kPi;

    m_BottomRadius = 6360000.0;

    DensityProfileLayer
        rayleigh_layer(0.0, 1.0, -1.0 / kRayleighScaleHeight, 0.0, 0.0);
    DensityProfileLayer mie_layer(0.0, 1.0, -1.0 / kMieScaleHeight, 0.0, 0.0);
    // Density profile increasing linearly from 0 to 1 between 10 and 25km, and
    // decreasing linearly from 1 to 0 between 25 and 40km. This is an approximate
    // profile from http://www.kln.ac.lk/science/Chemistry/Teaching_Resources/
    // Documents/Introduction%20to%20atmospheric%20chemistry.pdf (page 10).
    std::vector<DensityProfileLayer> ozone_density;
    ozone_density.push_back(
        DensityProfileLayer(25000.0, 0.0, 0.0, 1.0 / 15000.0, -2.0 / 3.0));
    ozone_density.push_back(
        DensityProfileLayer(0.0, 0.0, 0.0, -1.0 / 15000.0, 8.0 / 3.0));

    // FIXME(guillem): Be careful with these
    m_wavelengths.clear();
    m_solar_irradiance.clear();
    std::vector<double> rayleigh_scattering;
    std::vector<double> mie_scattering;
    std::vector<double> mie_extinction;
    std::vector<double> absorption_extinction;
    std::vector<double> ground_albedo;
    for (int l = kLambdaMin; l <= kLambdaMax; l += 10) {
        double lambda = static_cast<double>(l) * 1e-3;  // micro-meters
        double mie =
            kMieAngstromBeta / kMieScaleHeight * pow(lambda, -kMieAngstromAlpha);
        m_wavelengths.push_back(l);
        if (use_constant_solar_spectrum_) {
            m_solar_irradiance.push_back(kConstantSolarIrradiance);
        }
        else {
            m_solar_irradiance.push_back(kSolarIrradiance[(l - kLambdaMin) / 10]);
        }
        rayleigh_scattering.push_back(kRayleigh * pow(lambda, -4));
        mie_scattering.push_back(mie * kMieSingleScatteringAlbedo);
        mie_extinction.push_back(mie);
        absorption_extinction.push_back(use_ozone_ ?
            kMaxOzoneNumberDensity * kOzoneCrossSection[(l - kLambdaMin) / 10] :
            0.0);
        ground_albedo.push_back(kGroundAlbedo);
    }

    model_.reset(new Model(m_wavelengths, m_solar_irradiance, kSunAngularRadius,
        m_BottomRadius, kTopRadius, { rayleigh_layer }, rayleigh_scattering,
        { mie_layer }, mie_scattering, mie_extinction, kMiePhaseFunctionG,
        ozone_density, absorption_extinction, ground_albedo, max_sun_zenith_angle,
        kLengthUnitInMeters, use_luminance_ == Luminance::PRECOMPUTED ? 15 : 3,
        use_combined_textures_, use_half_precision_));
    model_->Init();

    /*
    <p>Then, it creates and compiles the vertex and fragment shaders used to render
    our demo scene, and link them with the <code>Model</code>'s atmosphere shader
    to get the final scene rendering program:
    */

    vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
    const char* const vertex_shader_source = kVertexShader;
    glShaderSource(vertex_shader_, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader_);

    const std::string fragment_shader_str =
        "#version 330\n" +
        std::string(use_luminance_ != Luminance::NONE ? "#define USE_LUMINANCE\n" : "") +
        "const float kLengthUnitInMeters = " +
        std::to_string(kLengthUnitInMeters) + ";\n" +
        demo_glsl;
    const char* fragment_shader_source = fragment_shader_str.c_str();
    fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader_);

    GLint success;
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

    // NOTE(guillem): Code added by me
    // Init mesh shader
    const char vertexShaderSource[] = R"(
        #version 330 core
        layout (location = 0) in vec3 m_Pos;
        layout (location = 1) in vec3 m_Normal;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        out vec3 w_Normal;
        out vec3 w_Pos;
        void main()
        {
            mat3 normalMatrix = inverse(transpose(mat3(model)));
            w_Normal = normalMatrix * m_Normal;
            w_Pos = (model * vec4(m_Pos, 1.0)).xyz;
            gl_Position = projection * view * vec4(w_Pos, 1.0);
            w_Normal = w_Normal.zxy;
            w_Pos = w_Pos.zxy;
        }
)";
    const char fragmentShaderSource[] = R"(
        #version 330 core

        #ifdef USE_LUMINANCE
        #define GetSolarRadiance GetSolarLuminance
        #define GetSkyRadiance GetSkyLuminance
        #define GetSkyRadianceToPoint GetSkyLuminanceToPoint
        #define GetSunAndSkyIrradiance GetSunAndSkyIlluminance
        #endif

        // Forward declarations
        vec3 GetSolarRadiance();
        vec3 GetSkyRadiance(vec3 camera, vec3 view_ray, float shadow_length,
            vec3 sun_direction, out vec3 transmittance);
        vec3 GetSkyRadianceToPoint(vec3 camera, vec3 point, float shadow_length,
            vec3 sun_direction, out vec3 transmittance);
        vec3 GetSunAndSkyIrradiance(
            vec3 p, vec3 normal, vec3 sun_direction, out vec3 sky_irradiance);

        const float PI = 3.14159265;
        const vec3 color = vec3(0.1, 0.1, 0.1);
        uniform float exposure;
        uniform vec3 white_point;
        uniform vec3 sun_direction;
        uniform vec3 earth_center;
        uniform vec3 camera_pos;
        in vec3 w_Pos;
        in vec3 w_Normal;
        out vec4 FragColor;
        /*void main()
        {
            vec3 albedo = vec3(0.8, 0.7, 0.1);
            FragColor = vec4(albedo * w_Normal.z, 1.0);
        }*/
        void main()
        {
            
            vec3 sky_irradiance;
            vec3 sun_irradiance = GetSunAndSkyIrradiance(w_Pos - earth_center, w_Normal, sun_direction, sky_irradiance);

            vec3 sphere_radiance = color * (1.0 / PI) * (sun_irradiance + sky_irradiance);

            vec3 transmittance;
            vec3 in_scatter = GetSkyRadianceToPoint(camera_pos.xyz - earth_center, w_Pos - earth_center, 0.0, sun_direction, transmittance);

            sphere_radiance = sphere_radiance * transmittance + in_scatter;

            sphere_radiance = pow(vec3(1,1,1) - exp(-sphere_radiance / white_point * exposure), vec3(1.0 / 2.2));

            FragColor = vec4(sphere_radiance, 1.0);
        }
)";

    m_meshProgram.SetVertexShaderSource(vertexShaderSource);
    m_meshProgram.SetFragmentShaderSource(fragmentShaderSource);
    m_meshProgram.AttachShader(model_->shader());
    m_meshProgram.Build();

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
    double white_point_r = 1.0;
    double white_point_g = 1.0;
    double white_point_b = 1.0;
    if (do_white_balance_) {
        Model::ConvertSpectrumToLinearSrgb(m_wavelengths, m_solar_irradiance,
            &white_point_r, &white_point_g, &white_point_b);
        double white_point = (white_point_r + white_point_g + white_point_b) / 3.0;
        white_point_r /= white_point;
        white_point_g /= white_point;
        white_point_b /= white_point;
    }
    glUniform3f(glGetUniformLocation(program_, "white_point"),
        white_point_r, white_point_g, white_point_b);
    glUniform3f(glGetUniformLocation(program_, "earth_center"),
        0.0, 0.0, -m_BottomRadius / kLengthUnitInMeters);
    glUniform2f(glGetUniformLocation(program_, "sun_size"),
        tan(kSunAngularRadius),
        cos(kSunAngularRadius));

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
    SetRenderingContext(camera);

    // NOTE: Transform from our camera approach (classical opengl) to their camera approach (mathematical approach)
    // Might be ok to move these to the camera class
    // IDEA: Create a compass widget to know in which direction we are looking at

    glm::mat4 permutation = glm::mat4(glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0), glm::vec4(1, 0, 0, 0), glm::vec4(0, 0, 0, 1));
    glm::vec4 cameraRight = permutation * glm::vec4(camera.GetRight(), 0.0f);
    glm::vec4 cameraForward = permutation * glm::vec4(camera.GetForward(), 0.0f);
    glm::vec4 cameraUp = permutation * glm::vec4(camera.GetUp(), 0.0f);
    glm::vec4 cameraPosition = permutation * glm::vec4(camera.GetPosition(), 1.0f);

    glm::mat4 model_from_view_ = glm::mat4(cameraRight, cameraUp, -cameraForward, cameraPosition);
    glUniform3fv(glGetUniformLocation(program_, "camera"), 1, glm::value_ptr(cameraPosition));
    glUniformMatrix4fv(glGetUniformLocation(program_, "model_from_view"), 1, GL_FALSE, glm::value_ptr(model_from_view_));
    glUniform1f(glGetUniformLocation(program_, "exposure"), use_luminance_ != Luminance::NONE ? exposure_ * 1e-5 : exposure_);

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
        glBindVertexArray(full_screen_quad_vao_);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        m_meshProgram.Use();
        m_meshProgram.SetMat4("model", glm::mat4(1.0f));
        m_meshProgram.SetMat4("view", camera.GetViewMatrix());
        m_meshProgram.SetMat4("projection", camera.GetProjectionMatrix());
        m_meshProgram.SetFloat("exposure", use_luminance_ != Luminance::NONE ? exposure_ * 1e-5 : exposure_);
        m_meshProgram.SetVec3("sun_direction", sunDirection);
        m_meshProgram.SetVec3("camera_pos", cameraPosition);
        glm::vec3 whitePoint = glm::vec3(1.0); // FIXME
        m_meshProgram.SetVec3("white_point", whitePoint);
        glm::vec3 earthCenter = glm::vec3(0.0f, 0.0f, -m_BottomRadius / kLengthUnitInMeters); // FIXME: Is this correct or should it be permutated (?)
        m_meshProgram.SetVec3("earth_center", earthCenter);
        // TODO: Draw another mesh
        // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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
        shouldRecomputeModel |= ImGui::Checkbox("Use constant solar spectrum", &use_constant_solar_spectrum_);
        shouldRecomputeModel |= ImGui::Checkbox("Use ozone", &use_ozone_);
        shouldRecomputeModel |= ImGui::Checkbox("Use combined textures", &use_combined_textures_);
        shouldRecomputeModel |= ImGui::Checkbox("Use half precision", &use_half_precision_);

        ImGui::Text("Luminance");
        shouldRecomputeModel |= ImGui::RadioButton("None", reinterpret_cast<int*>(&use_luminance_), static_cast<int>(Luminance::NONE));
        ImGui::SameLine();
        shouldRecomputeModel |= ImGui::RadioButton("Approximate", reinterpret_cast<int*>(&use_luminance_), static_cast<int>(Luminance::APPROXIMATE));
        ImGui::SameLine();
        shouldRecomputeModel |= ImGui::RadioButton("Precomputed", reinterpret_cast<int*>(&use_luminance_), static_cast<int>(Luminance::PRECOMPUTED));

        ImGui::Checkbox("Do white balance", &do_white_balance_);

        float exposure_f = exposure_;
        ImGui::DragFloat("Exposure", &exposure_f, 0.1f, 2.0f, 1000.0f, "%.3f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);
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
}

void PhysicalSky::OnMouseClick(int button, int action, int mods) {
    is_mouse_button_pressed_ = (action == GLFW_PRESS);
    is_ctrl_key_pressed_ = (mods & GLFW_MOD_CONTROL);
}

bool PhysicalSky::OnMouseMovement(glm::vec2 movement) {
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
