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

/*<h2>atmosphere/demo/demo.h</h2>

<p>Our demo application consists of a single class, whose header is defined
below. Besides a constructor and an initialization method, this class has one
method per user interface event, and a few fields to store the current rendering
options, the current camera and Sun parameters, as well as references to the
atmosphere model and to the GLSL program, vertex buffers and text renderer used
to render the scene and the help messages:
*/

#pragma once

#include "Camera.h"
#include "ShaderProgram.h"
class Window;
#include "Mesh.h"
#include "PhysicalSky.h"
#include "Coordinates.h"

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <memory>

class Scene {
public:
    Scene();
    ~Scene();

  void Init(Window* m_window);
  void InitModel();
  void InitShaders();
  void Update();
  void Render(const Camera& camera);
  void RenderMeshes(const Camera& camera);
  void OnMouseClick(int button, int action, int mods);
  bool OnCursorMovement(glm::vec2 movement);
  void SetView(double view_distance_meters, double view_zenith_angle_radians,
      double view_azimuth_angle_radians, double sun_zenith_angle_radians,
      double sun_azimuth_angle_radians);
 private:
  double sun_zenith_angle_radians_;
  double sun_azimuth_angle_radians_;

  bool is_ctrl_key_pressed_;

  // NOTE(guillem): Added by me
  bool is_mouse_button_pressed_;

  // Mesh and shader
  std::unique_ptr<Mesh> m_mesh;
  ShaderProgram m_meshShader;

  bool m_useDemo;

  PhysicalSky m_physicalSky;
  Coordinates m_coordinates;
};