#pragma once

#include <min-ray/common.h>
#include <nanogui/screen.h>

namespace min::ray {

class NoriScreen : public nanogui::Screen {
 public:
  NoriScreen(const ImageBlock &block);
  virtual ~NoriScreen();

  void drawContents();

 private:
  const ImageBlock &m_block;
  nanogui::GLShader *m_shader = nullptr;
  nanogui::Slider *m_slider = nullptr;
  uint32_t m_texture = 0;
  float m_scale = 1.f;
};

}  // namespace min::ray
