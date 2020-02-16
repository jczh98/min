#pragma once

#include <min/visual/defs.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace min {
class PreviewGUI {
  const std::shared_ptr<Film> &film;
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
  PreviewGUI(const std::shared_ptr<Film> &film) : film(film) {}
  ~PreviewGUI() {}
  void Init();
  void Mainloop(int refresh = 50);
  void Draw();
  void Shutdown();
};
}