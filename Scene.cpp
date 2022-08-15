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

#include "Scene.h"

#include "Window.h"

#include <imgui.h>

#include <glad/glad.h>

#include <glm/gtc/constants.hpp>

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

Scene::Scene()
    : sun_zenith_angle_radians_(glm::half_pi<float>())
    , sun_azimuth_angle_radians_(glm::pi<float>())
    , m_mesh()
    , m_meshShader()
    , m_useDemo(true)
{
    m_mesh = std::make_unique<Mesh>("D:/Escritorio/sphere.gltf");
}

/*
<p>The destructor is even simpler:
*/

Scene::~Scene() {

}

void Scene::Init(Window* window) {
    is_mouse_button_pressed_ = false;
    InitModel();
}


/*
<p>The "real" initialization work, which is specific to our atmosphere model,
is done in the following method. It starts with the creation of an atmosphere
<code>Model</code> instance, with parameters corresponding to the Earth
atmosphere:
*/

void Scene::InitModel() {
    
}

// TODO
void Scene::InitShaders()
{
    //ShaderStage meshVertexShader = ShaderStage();
    //meshVertexShader.Create(ShaderType::VERTEX);
    //meshVertexShader.Compile("D:/dev/miri-tfm/resources/shaders/mesh.vert", "D:/dev/miri-tfm/resources/shaders/");

    //ShaderStage meshFragmentShader = ShaderStage();
    //meshFragmentShader.Create(ShaderType::FRAGMENT);
    //meshFragmentShader.Compile("D:/dev/miri-tfm/resources/shaders/mesh.frag", "D:/dev/miri-tfm/resources/shaders/");

    //m_meshShader.Create();
    //m_meshShader.AttachShader(meshVertexShader.m_id);
    //m_meshShader.AttachShader(meshFragmentShader.m_id);
    //m_meshShader.AttachShader(model_->shader()); // TODO: Get from m_physicalSky
    //m_meshShader.Build();
}

void Scene::Update()
{
    m_physicalSky.Update(); // TODO: If model is recomputed, initshaders again

    if (ImGui::Begin("Scene Settings"))
    {
        ImGui::Checkbox("Use Demo Shader", &m_useDemo);
        ImGui::Text("Predefined Views");
        if (ImGui::Button("1")) SetView(9000.0, 1.47, 0.0, 1.3, 3.0);
        ImGui::SameLine();
        if (ImGui::Button("2")) SetView(9000.0, 1.47, 0.0, 1.564, -3.0);
        ImGui::SameLine();
        if (ImGui::Button("3")) SetView(7000.0, 1.57, 0.0, 1.54, -2.96);
        ImGui::SameLine();
        if (ImGui::Button("4")) SetView(7000.0, 1.57, 0.0, 1.328, -3.044);
        ImGui::SameLine();
        if (ImGui::Button("5")) SetView(9000.0, 1.39, 0.0, 1.2, 0.7);
        ImGui::SameLine();
        if (ImGui::Button("6")) SetView(9000.0, 1.5, 0.0, 1.628, 1.05);
        ImGui::SameLine();
        if (ImGui::Button("7")) SetView(7000.0, 1.43, 0.0, 1.57, 1.34);
        ImGui::SameLine();
        if (ImGui::Button("8")) SetView(2.7e6, 0.81, 0.0, 1.57, 2.0);
        ImGui::SameLine();
        if (ImGui::Button("9")) SetView(1.2e7, 0.0, 0.0, 0.93, -2.0);
    }
    ImGui::End();
}

/*
<p>The scene rendering method simply sets the uniforms related to the camera
position and to the Sun direction, and then draws a full screen quad (and
optionally a help screen).
*/

void Scene::Render(const Camera& camera) {
    // if (glGetError() != GL_NO_ERROR) std::cerr << "[OpenGL] E: After rendering UI." << std::endl;

    // NOTE: Transform from our camera approach (classical opengl) to their camera approach (mathematical approach)
    // Might be ok to move these to the camera class
    // IDEA: Create a compass widget to know in which direction we are looking at
    glm::vec2 sunAngles = glm::vec2(sun_azimuth_angle_radians_, sun_zenith_angle_radians_); // TODO: Make member of the class
    if (m_useDemo)
    {
        m_physicalSky.RenderDemo(camera, sunAngles);
    }
    else
    {
        RenderMeshes(camera);
        m_physicalSky.Render(camera, sunAngles);
    }
}

// TODO
void Scene::RenderMeshes(const Camera& camera)
{
   /* glm::vec2 sunAngles = glm::vec2(sun_azimuth_angle_radians_, sun_zenith_angle_radians_);
    glm::vec2 sunCos = glm::cos(sunAngles);
    glm::vec2 sunSin = glm::sin(sunAngles);
    m_meshShader.Use();
    glm::mat4 model = glm::mat4(1.0f);
    float control = m_planetRadius;
    model = glm::translate(model, glm::vec3(0.0f, -control, 0.0f));
    model = glm::scale(model, glm::vec3(control));
    m_meshShader.SetMat4("model", model);
    m_meshShader.SetMat4("view", camera.GetViewMatrix());
    m_meshShader.SetMat4("projection", camera.GetProjectionMatrix());
    m_meshShader.SetVec3("w_LightDir", glm::vec3(sunSin.y * sunSin.x, sunCos.y, sunSin.y * sunCos.x));
    m_meshShader.SetVec3("w_CameraPos", camera.GetPosition());
    m_meshShader.SetVec3("w_PlanetPos", glm::vec3(0.0f, -m_planetRadius, 0.0f));
    m_meshShader.SetVec3("albedo", m_groundAlbedo);
    model_->SetProgramUniforms(m_meshShader.m_id, 0, 1, 2, 3);
    m_mesh->JustRender(camera);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
    m_meshShader.SetMat4("model", model);
    m_meshShader.SetVec3("albedo", glm::vec3(0.8f, 0.8f, 0.8f));
    m_mesh->JustRender(camera);*/
}

/*
<p>The other event handling methods are also straightforward, and do not
interact with the atmosphere model:
*/

void Scene::OnMouseClick(int button, int action, int mods) {
    is_mouse_button_pressed_ = (action == GLFW_PRESS);
    is_ctrl_key_pressed_ = (mods & GLFW_MOD_CONTROL);
}

bool Scene::OnCursorMovement(glm::vec2 movement) {
    if (!is_mouse_button_pressed_) return false;

    constexpr double kScale = 500.0;
    if (is_ctrl_key_pressed_)
    {
        sun_zenith_angle_radians_ -= movement.y / kScale;
        sun_zenith_angle_radians_ = glm::max(0.0f, glm::min(glm::pi<float>(), static_cast<float>(sun_zenith_angle_radians_)));
        sun_azimuth_angle_radians_ += movement.x / kScale;
        return true;
    }
    return false;
}

void Scene::SetView(double view_distance_meters,
    double view_zenith_angle_radians, double view_azimuth_angle_radians,
    double sun_zenith_angle_radians, double sun_azimuth_angle_radians) {
    // NOTE: This should set the camera settings
    //view_distance_meters_ = view_distance_meters;
    // view_zenith_angle_radians_ = view_zenith_angle_radians;
    // view_azimuth_angle_radians_ = view_azimuth_angle_radians;
    sun_zenith_angle_radians_ = sun_zenith_angle_radians;
    sun_azimuth_angle_radians_ = sun_azimuth_angle_radians;
}
