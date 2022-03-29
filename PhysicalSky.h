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
#include "Program.h"
class Window;

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>

#include <atmosphere/model.h>

class PhysicalSky {
public:
    PhysicalSky(int viewport_width, int viewport_height);
    ~PhysicalSky();

  const atmosphere::Model& model() const { return *model_; }
  const GLuint vertex_shader() const { return vertex_shader_; }
  const GLuint fragment_shader() const { return fragment_shader_; }
  const GLuint program() const { return program_; }

  void Initialize(Window* m_window);
  void InitResources();
  void InitModel();
  void SetRenderingContext(const Camera& camera) const;
  void Render(const Camera& camera);
  void OnMouseClick(int button, int action, int mods);
  bool OnMouseMovement(glm::vec2 movement);
  void SetView(double view_distance_meters, double view_zenith_angle_radians,
      double view_azimuth_angle_radians, double sun_zenith_angle_radians,
      double sun_azimuth_angle_radians, double exposure);

  void RenderUi();

 private:
  enum class Luminance {
    // Render the spectral radiance at kLambdaR, kLambdaG, kLambdaB.
    NONE,
    // Render the sRGB luminance, using an approximate (on the fly) conversion
    // from 3 spectral radiance values only (see section 14.3 in <a href=
    // "https://arxiv.org/pdf/1612.04336.pdf">A Qualitative and Quantitative
    //  Evaluation of 8 Clear Sky Models</a>).
    APPROXIMATE,
    // Render the sRGB luminance, precomputed from 15 spectral radiance values
    // (see section 4.4 in <a href=
    // "http://www.oskee.wz.cz/stranka/uploads/SCCG10ElekKmoch.pdf">Real-time
    //  Spectral Scattering in Large-scale Natural Participating Media</a>).
    PRECOMPUTED
  };


  bool use_constant_solar_spectrum_;
  bool use_ozone_;
  bool use_combined_textures_;
  bool use_half_precision_;
  Luminance use_luminance_;
  bool do_white_balance_;
  bool show_help_;

  std::unique_ptr<atmosphere::Model> model_;
  GLuint vertex_shader_;
  GLuint fragment_shader_;
  GLuint program_;
  GLuint full_screen_quad_vao_;
  GLuint full_screen_quad_vbo_;
  int window_id_;

  double sun_zenith_angle_radians_;
  double sun_azimuth_angle_radians_;
  double exposure_;

  bool is_ctrl_key_pressed_;

  // NOTE(guillem): Added by me
  std::vector<double> m_wavelengths;
  std::vector<double> m_solar_irradiance;
  double m_BottomRadius;
  bool is_mouse_button_pressed_;
  Program m_meshProgram;
};
