/***********************************************************************
*
* Copyright (c) 2019 Barbara Geller
* Copyright (c) 2019 Ansel Sermersheim
*
* This file is part of CsPaint.
*
* CsPaint is free software, released under the BSD 2-Clause license.
* For license details refer to LICENSE provided with this project.
*
* CopperSpice is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* https://opensource.org/licenses/BSD-2-Clause
*
***********************************************************************/

#include <demo.h>

constexpr float hexToFloat(uint8_t input)
{
   return input / 255.0f;
}

static std::vector<Vertex> load_vertices(std::string filename, glm::vec3 color);
static std::vector<Vertex> render_string(std::string text, glm::vec3 color);

const std::multimap<int, std::vector<Vertex>> &init_vertex_data()
{
   static const std::multimap<int, std::vector<Vertex>> retval =
      {{0, load_vertices(DEMO_RESOURCE_DIR "/pot.stl", glm::vec3(hexToFloat(0xB8), hexToFloat(0x73), hexToFloat(0x33)))},
       {1, load_vertices(DEMO_RESOURCE_DIR "/shaker.stl", glm::vec3(hexToFloat(0xF8), hexToFloat(0xDF), hexToFloat(0xA1)))},
       {2, load_vertices(DEMO_RESOURCE_DIR "/table.stl", glm::vec3(hexToFloat(0xc1), hexToFloat(0x9a), hexToFloat(0x6b)))},
       {2, load_vertices(DEMO_RESOURCE_DIR "/tabletop.stl", glm::vec3(hexToFloat(0xF1), hexToFloat(0xFa), hexToFloat(0x6b)))},
       {3, load_vertices(DEMO_RESOURCE_DIR "/frame.stl", glm::vec3(hexToFloat(0x20), hexToFloat(0x20), hexToFloat(0x50)))},
       {3, load_vertices(DEMO_RESOURCE_DIR "/chalkboard.stl", glm::vec3(hexToFloat(0x05), hexToFloat(0x05), hexToFloat(0x05)))}};

   return retval;
}

const std::multimap<int, std::vector<Vertex>> &init_text_data()
{
   static const std::multimap<int, std::vector<Vertex>> retval =
      {{4, render_string("CppCon 2019", glm::vec3(hexToFloat(0xFF), hexToFloat(0xFF), hexToFloat(0x00)))},
       {5, render_string("GRAPHICS DEMO", glm::vec3(hexToFloat(0xFF), hexToFloat(0xFF), hexToFloat(0x00)))}};

   return retval;
}

namespace vertex_shader
{
#include "resources/shader.vert.h"
};

namespace fragment_shader
{
#include "resources/shader.frag.h"
};

namespace text_vertex_shader
{
#include "resources/text.vert.h"
};

namespace text_fragment_shader
{
#include "resources/text.frag.h"
};

namespace font_texture
{
#include "resources/DejaVuSansMono.h"
};

const std::vector<uint32_t> vertexShaderData{std::begin(vertex_shader::shader), std::end(vertex_shader::shader)};
const std::vector<uint32_t> fragmentShaderData{std::begin(fragment_shader::shader), std::end(fragment_shader::shader)};
const std::vector<uint32_t> textVertexShaderData{std::begin(text_vertex_shader::shader), std::end(text_vertex_shader::shader)};
const std::vector<uint32_t> textFragmentShaderData{std::begin(text_fragment_shader::shader),
                                                   std::end(text_fragment_shader::shader)};
const std::vector<uint8_t> fontTextureData{std::begin(font_texture::MagickImage), std::end(font_texture::MagickImage)};

glm::vec3 polar_coordinate(float rho, float theta, float z)
{
   return glm::vec3{theta * sin(rho), theta * cos(rho), z};
}

glm::mat4 transform_matrix(glm::vec3 rotation)
{
   // position of the copper pot
   glm::mat4 Model = glm::mat4(1.0f);
   Model           = glm::scale(Model, glm::vec3(0.30f));
   Model           = glm::translate(Model, glm::vec3(-0.60f, 0.65f, 0.0f));
   Model           = glm::rotate(Model, rotation.x, glm::vec3(-1.0f, 0.0f, 0.0f));
   Model           = glm::rotate(Model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));

   return Model;
}

Uniform calculate_uniform(glm::mat4 transform_matrix, float aspect_ratio, float zoom_factor)
{
   Uniform uniformData;

   glm::vec3 cameraPositions[2] = {{0, -0.75, -3}, {0.328, -0.040, .75}};

   glm::vec3 lookPositions[2] = {{
                                    0.0,
                                    0.0,
                                    0.0,
                                 },
                                 {0.328, 0.030, 2}};

   glm::vec3 cameraPosition = glm::mix(cameraPositions[0], cameraPositions[1], zoom_factor);
   glm::vec3 lookPosition   = glm::mix(lookPositions[0], lookPositions[1], zoom_factor);

   uniformData.viewMatrix = glm::lookAt(cameraPosition, lookPosition, glm::vec3(0, 1, 0));

   uniformData.modelMatrix[0] = transform_matrix;

   // position of the table
   auto tableLocation = glm::mat4(1.0f);
   tableLocation      = glm::translate(tableLocation, glm::vec3(-0.75f, 0.05, 2));
   tableLocation      = glm::scale(tableLocation, glm::vec3(0.75f));
   tableLocation      = glm::rotate(tableLocation, glm::pi<float>() * -0.15f, glm::vec3(0.0f, 1.0f, 0.0f));

   // position of the salt and pepper
   auto shakerMatrix = glm::mat4(1.0f);
   shakerMatrix      = glm::translate(shakerMatrix, glm::vec3(-0.3f, -0.11f, 0.0f));
   shakerMatrix      = glm::scale(shakerMatrix, glm::vec3(0.05f));
   shakerMatrix      = glm::rotate(shakerMatrix, glm::pi<float>() * 0.30f, glm::vec3(0.0f, 1.0f, 0.0f));

   // shaker
   uniformData.modelMatrix[1] = tableLocation * shakerMatrix;

   // table
   uniformData.modelMatrix[2] = tableLocation * glm::scale(glm::mat4(1.0f), glm::vec3(0.7f));

   // position of the chalkboard
   auto boardMatrix           = glm::mat4(1.0f);
   boardMatrix                = glm::translate(boardMatrix, glm::vec3(0.55f, -0.05f, 1));
   boardMatrix                = glm::scale(boardMatrix, glm::vec3(0.3f));
   boardMatrix                = glm::rotate(boardMatrix, glm::pi<float>() * 1.50f, glm::vec3(0.0f, 1.0f, 0.0f));
   boardMatrix                = glm::rotate(boardMatrix, glm::pi<float>() * 0.05f, glm::vec3(0.0f, 0.0f, 1.0f));
   uniformData.modelMatrix[3] = boardMatrix;

   // position of the text
   auto textMatrix = glm::mat4(1.0f);
   textMatrix      = glm::translate(textMatrix, glm::vec3(0.0f, -0.05f, .99));
   textMatrix      = glm::scale(textMatrix, glm::vec3(0.12f));
   textMatrix      = glm::rotate(textMatrix, glm::pi<float>() * -0.05f, glm::vec3(1.0f, 0.0f, 0.0f));
   textMatrix      = glm::rotate(textMatrix, glm::pi<float>() * 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));
   textMatrix      = glm::translate(textMatrix, glm::vec3(-7.4f, 1.5f, 0.0f));

   // text
   uniformData.modelMatrix[4] = textMatrix;

   // move second line down
   uniformData.modelMatrix[5] = glm::translate(textMatrix, glm::vec3(-0.4f, -1.0f, 0.0f));

   // camera
   uniformData.projMatrix = glm::perspective(glm::pi<float>() * 0.10f, aspect_ratio, .1f, 10.0f);

   for (int i = 0; i < NUM_MODELS; ++i) {
      uniformData.normalMatrix[i] = glm::inverseTranspose(uniformData.viewMatrix * uniformData.modelMatrix[i]);
   }
   uniformData.lightPosition = glm::vec3{30, -60, -30};

   return uniformData;
}

std::vector<Vertex> load_vertices(std::string filename, glm::vec3 color)
{
   std::vector<Vertex> retval;

   auto model_data = CsPaint::io::load_stl(filename);

   for (const auto &input : model_data) {
      glm::vec3 tmp{input.x, input.y, input.z};
      glm::vec3 tmp2{input.nx, input.ny, input.nz};

      retval.push_back(Vertex{tmp, tmp2, color});
   }

   if (retval.empty()) {
      printf("No vertices loaded in %s\n", filename.c_str());
   }

   return retval;
}

std::vector<Vertex> render_string(std::string text, glm::vec3 color)
{
   std::vector<Vertex> retval;

   int cnt             = 0;
   const float advance = 0.45;

   for (const auto &ch : text) {

      int column = ch & 0x0F;
      int row    = (ch & 0xF0) >> 4;

      row -= 2; // First two rows are unprintable
      if (row >= 6) {
         row -= 2; // Rows 8 and 9 are unprintable
      }

      const float charWidth  = 1.0f / 16; // 16 columns across
      const float charHeight = 1.0f / 12; // 12 rows high
      const float margin     = 0.001f;

      retval.push_back(Vertex{glm::vec3{0.05f + cnt * advance, -0.95f, 0}, glm::vec3{0, 0, 1}, color,
                              glm::vec2{column * charWidth + margin, (row + 1) * charHeight - margin}});
      retval.push_back(Vertex{glm::vec3{0.95f + cnt * advance, -0.95f, 0}, glm::vec3{0, 0, 1}, color,
                              glm::vec2{(column + 1) * charWidth - margin, (row + 1) * charHeight - margin}});
      retval.push_back(Vertex{glm::vec3{0.05f + cnt * advance, -0.05f, 0}, glm::vec3{0, 0, 1}, color,
                              glm::vec2{column * charWidth + margin, row * charHeight + margin}});

      retval.push_back(Vertex{glm::vec3{0.95f + cnt * advance, -0.95f, 0}, glm::vec3{0, 0, 1}, color,
                              glm::vec2{(column + 1) * charWidth - margin, (row + 1) * charHeight - margin}});
      retval.push_back(Vertex{glm::vec3{0.95f + cnt * advance, -0.05f, 0}, glm::vec3{0, 0, 1}, color,
                              glm::vec2{(column + 1) * charWidth - margin, row * charHeight + margin}});
      retval.push_back(Vertex{glm::vec3{0.05f + cnt * advance, -0.05f, 0}, glm::vec3{0, 0, 1}, color,
                              glm::vec2{column * charWidth + margin, row * charHeight + margin}});

      ++cnt;
   }

   if (retval.empty()) {
      printf("No text generated for %s\n", text.c_str());
   }

   return retval;
}
