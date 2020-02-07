#pragma once

#include <min-ray/common.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace min::ray {
class PreviewGUI {
  const ImageBlock &block;
  uint32_t texture = 0;
  float scale = 1.0f;
  bool mainloop_active = false;
  bool shutdown = false;
  float pixel_ration;
  GLFWwindow *window = nullptr;
  GLuint vertex_shader, frag_shader, program_shader;
  GLuint vao;
  void InitDrawcalls();
 public:
  PreviewGUI(const ImageBlock &block) : block(block) {
  }
  ~PreviewGUI() {}
  void Init();
  void Mainloop(int refresh = 50);
  void Draw();
  void Shutdown();
};
}