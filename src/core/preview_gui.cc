

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <thread>
#include <min-ray/preview_gui.h>
#include <min-ray/block.h>
#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

namespace min::ray {
static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

/* Calculate pixel ratio for hi-dpi devices. */
static float get_pixel_ratio(GLFWwindow *window) {
#if defined(_WIN32)
  HWND hWnd = glfwGetWin32Window(window);
  HMONITOR monitor = nullptr;
#if defined(MONITOR_DEFAULTTONEAREST)
  monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
#else
  static HMONITOR (WINAPI *MonitorFromWindow_)(HWND, DWORD) = nullptr;
        static bool MonitorFromWindow_tried = false;
        if (!MonitorFromWindow_tried) {
            auto user32 = LoadLibrary(TEXT("user32"));
            if (user32)
                MonitorFromWindow_ = (decltype(MonitorFromWindow_)) GetProcAddress(user32, "MonitorFromWindow");
            MonitorFromWindow_tried = true;
        }
        if (MonitorFromWindow_)
            monitor = MonitorFromWindow_(hWnd, 2);
#endif  // defined(MONITOR_DEFAULTTONEAREST)
  /* The following function only exists on Windows 8.1+, but we don't want to make that a dependency */
  static HRESULT (WINAPI *GetDpiForMonitor_)(HMONITOR, UINT, UINT*, UINT*) = nullptr;
  static bool GetDpiForMonitor_tried = false;

  if (!GetDpiForMonitor_tried) {
    auto shcore = LoadLibrary(TEXT("shcore"));
    if (shcore)
      GetDpiForMonitor_ = (decltype(GetDpiForMonitor_)) GetProcAddress(shcore, "GetDpiForMonitor");
    GetDpiForMonitor_tried = true;
  }

  if (GetDpiForMonitor_ && monitor) {
    uint32_t dpiX, dpiY;
    if (GetDpiForMonitor_(monitor, 0 /* effective DPI */, &dpiX, &dpiY) == S_OK)
      return dpiX / 96.0;
  }
  return 1.f;
#elif defined(__linux__)
  (void) window;

    float ratio = 1.0f;
    FILE *fp;
    /* Try to read the pixel ratio from KDEs config */
    auto currentDesktop = std::getenv("XDG_CURRENT_DESKTOP");
    if (currentDesktop && currentDesktop == std::string("KDE")) {
        fp = popen("kreadconfig5 --group KScreen --key ScaleFactor", "r");
        if (!fp)
            return 1;

        if (fscanf(fp, "%f", &ratio) != 1)
            return 1;
    } else {
        /* Try to read the pixel ratio from GTK */
        fp = popen("gsettings get org.gnome.desktop.interface scaling-factor", "r");
        if (!fp)
            return 1;

        int ratioInt = 1;
        if (fscanf(fp, "uint32 %i", &ratioInt) != 1)
            return 1;
        ratio = ratioInt;
    }
    if (pclose(fp) != 0)
        return 1;
    return ratio >= 1 ? ratio : 1;

#else
    Vector2i fbSize, size;
    glfwGetFramebufferSize(window, &fbSize[0], &fbSize[1]);
    glfwGetWindowSize(window, &size[0], &size[1]);
    return (float)fbSize[0] / (float)size[0];
#endif
}

static GLuint createShader_helper(GLint type, const std::string &name,
                                  const std::string &defines,
                                  std::string shader_string) {
  if (shader_string.empty())
    return (GLuint) 0;

  if (!defines.empty()) {
    if (shader_string.length() > 8 && shader_string.substr(0, 8) == "#version") {
      std::istringstream iss(shader_string);
      std::ostringstream oss;
      std::string line;
      std::getline(iss, line);
      oss << line << std::endl;
      oss << defines;
      while (std::getline(iss, line))
        oss << line << std::endl;
      shader_string = oss.str();
    } else {
      shader_string = defines + shader_string;
    }
  }

  GLuint id = glCreateShader(type);
  const char *shader_string_const = shader_string.c_str();
  glShaderSource(id, 1, &shader_string_const, nullptr);
  glCompileShader(id);

  GLint status;
  glGetShaderiv(id, GL_COMPILE_STATUS, &status);

  if (status != GL_TRUE) {
    char buffer[512];
    std::cerr << "Error while compiling ";
    if (type == GL_VERTEX_SHADER)
      std::cerr << "vertex shader";
    else if (type == GL_FRAGMENT_SHADER)
      std::cerr << "fragment shader";
    else if (type == GL_GEOMETRY_SHADER)
      std::cerr << "geometry shader";
    std::cerr << " \"" << name << "\":" << std::endl;
    std::cerr << shader_string << std::endl << std::endl;
    glGetShaderInfoLog(id, 512, nullptr, buffer);
    std::cerr << "Error: " << std::endl << buffer << std::endl;
    throw std::runtime_error("Shader compilation failed!");
  }

  return id;
}

void PreviewGUI::Init() {
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return;
  // Decide GL+GLSL versions
#if __APPLE__
  // GL 3.2 + GLSL 150
      const char* glsl_version = "#version 150";
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

  // Create window with graphics context
  const Vector2i size = block.GetSize();
  window = glfwCreateWindow(size.x(), size.y(), "Preview", NULL, NULL);
  if (window == NULL)
    return;
  pixel_ration = get_pixel_ratio(window);
  if (pixel_ration != 1)
    glfwSetWindowSize(window, size.x() * pixel_ration, size.y() * pixel_ration);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync
  // Initialize OpenGL loader
  bool err = !gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  if (err) {
    fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    return;
  }
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void) io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  InitDrawcalls();
}

void PreviewGUI::InitDrawcalls() {
  const std::string vertex_shader_source = "#version 330\n"
                                    "in vec2 position;\n"
                                    "out vec2 uv;\n"
                                    "void main() {\n"
                                    "    gl_Position = vec4(position.x*2-1, position.y*2-1, 0.0, 1.0);\n"
                                    "    uv = vec2(position.x, 1-position.y);\n"
                                    "}";
  const std::string frag_shader_source =       "#version 330\n"
                                        "uniform sampler2D source;\n"
                                        "uniform float scale;\n"
                                        "in vec2 uv;\n"
                                        "out vec4 out_color;\n"
                                        "float toSRGB(float value) {\n"
                                        "    if (value < 0.0031308)\n"
                                        "        return 12.92 * value;\n"
                                        "    return 1.055 * pow(value, 0.41666) - 0.055;\n"
                                        "}\n"
                                        "void main() {\n"
                                        "    vec4 color = texture(source, uv);\n"
                                        "    color *= scale / color.w;\n"
                                        "    out_color = vec4(toSRGB(color.r), toSRGB(color.g), toSRGB(color.b), 1);\n"
                                        "}";
  glGenVertexArrays(1, &vao);
  vertex_shader = createShader_helper(GL_VERTEX_SHADER, "", "", vertex_shader_source);
  frag_shader = createShader_helper(GL_FRAGMENT_SHADER, "", "", frag_shader_source);
  if (!vertex_shader || !frag_shader) {
    return;
  }
  program_shader = glCreateProgram();
  glAttachShader(program_shader, vertex_shader);
  glAttachShader(program_shader, frag_shader);
  glLinkProgram(program_shader);
  GLint status;
  glGetProgramiv(program_shader, GL_LINK_STATUS, &status);

  if (status != GL_TRUE) {
    char buffer[512];
    glGetProgramInfoLog(program_shader, 512, nullptr, buffer);
    std::cerr << "Linker error : " << std::endl << buffer << std::endl;
    program_shader = 0;
    throw std::runtime_error("Shader linking failed!");
  }

  MatrixXu indices(3, 2);
  indices.col(0) << 0, 1, 2;
  indices.col(1) << 2, 3, 0;

  MatrixXf positions(2, 4);
  positions.col(0) << 0, 0;
  positions.col(1) << 1, 0;
  positions.col(2) << 1, 1;
  positions.col(3) << 0, 1;

  glUseProgram(program_shader);
  glBindVertexArray(vao);
  // indices
  GLuint index_buffer;
  glGenBuffers(1, &index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(typename MatrixXu::Scalar), indices.data(), GL_DYNAMIC_DRAW);
  // positions
  int attrib_id = glGetAttribLocation(program_shader, "position");
  GLuint pos_buffer;
  glGenBuffers(1, &pos_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, pos_buffer);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(typename MatrixXf::Scalar), positions.data(), GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(attrib_id);
  glVertexAttribPointer(attrib_id, positions.rows(), GL_FLOAT, 0, 0, 0);
  // texture
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}
void PreviewGUI::Mainloop(int refresh) {
  if (mainloop_active)
    throw std::runtime_error("Main loop is already running!");

  mainloop_active = true;

  std::thread refresh_thread;
  if (refresh > 0) {
    /* If there are no mouse/keyboard events, try to refresh the
       view roughly every 50 ms (default); this is to support animations
       such as progress bars while keeping the system load
       reasonably low */
    refresh_thread = std::thread(
        [=]() {
          std::chrono::milliseconds time(refresh);
          while (mainloop_active) {
            std::this_thread::sleep_for(time);
            glfwPostEmptyEvent();
          }
        }
    );
  }

  try {
    while (mainloop_active) {
      if (glfwWindowShouldClose(window)) {
        mainloop_active = false;
        break;
      }
      Draw();

      if (shutdown) {
        /* Give up if there was nothing to draw */
        mainloop_active = false;
        break;
      }

      /* Wait for mouse/keyboard or empty refresh events */
      glfwWaitEvents();
    }

    /* Process events once more */
    glfwPollEvents();
  }
  catch (const std::exception &e) {
    std::cerr << "Caught exception in main loop: " << e.what() << std::endl;
    mainloop_active = false;
  }

  if (refresh > 0)
    refresh_thread.join();
}

void PreviewGUI::Draw() {
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);
  // Draw contents
  block.Lock();
  int borderSize = block.GetBorderSize();
  const Vector2i &size = block.GetSize();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, (GLint)block.cols());
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size.x(), size.y(),
               0, GL_RGBA, GL_FLOAT, (uint8_t *)block.data() + (borderSize * block.cols() + borderSize) * sizeof(Color4f));

  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  block.Unlock();

  glViewport(0, 0, GLsizei(pixel_ration * size[0]),
             GLsizei(pixel_ration * size[1]));
  glUseProgram(program_shader);
  glBindVertexArray(vao);
  const std::string scale_name = "scale";
  GLint scale_id = glGetUniformLocation(program_shader, scale_name.c_str());
  glUniform1f(scale_id, scale);
  GLint source_id = glGetUniformLocation(program_shader, "source");
  glUniform1i(source_id, 0);
  glDrawElements(GL_TRIANGLES, (GLsizei) 2 * 3, GL_UNSIGNED_INT, (const void *) 0);

  int display_w, display_h;
  // Rendering
  ImGui::Render();
  glfwGetFramebufferSize(window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  glfwSwapBuffers(window);
}

void PreviewGUI::Shutdown() {
  shutdown = true;
  glDeleteTextures(1, &texture);
  glDeleteBuffers(1, &vao);
  glDeleteProgram(program_shader);
  glDeleteShader(vertex_shader);
  glDeleteShader(frag_shader);
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
}

}