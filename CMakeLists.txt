cmake_minimum_required (VERSION 2.8.10)

project(min-ray)

add_subdirectory(ext ext_build)

include_directories(
  # Nori include files
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  # tinyformat string formatting library
  ${TFM_INCLUDE_DIR}
  # Eigen linear algebra library
  ${EIGEN_INCLUDE_DIR}
  # OpenEXR high dynamic range bitmap library
  ${OPENEXR_INCLUDE_DIRS}
  # Intel Thread Building Blocks
  ${TBB_INCLUDE_DIR}
  # Pseudorandom number generator
  ${PCG32_INCLUDE_DIR}
  # PugiXML parser
  ${PUGIXML_INCLUDE_DIR}
  # GLFW library for OpenGL context creation
  ${GLFW_INCLUDE_DIR}
  # GLEW library for accessing OpenGL functions
  ${GLEW_INCLUDE_DIR}
  # NanoVG drawing library
  ${NANOVG_INCLUDE_DIR}
  # NanoGUI user interface library
  ${NANOGUI_INCLUDE_DIR}
  ${NANOGUI_EXTRA_INCS}
  # Portable filesystem API
  ${FILESYSTEM_INCLUDE_DIR}
  # STB Image Write
  ${STB_IMAGE_WRITE_INCLUDE_DIR}
)

# The following lines build the main executable. If you add a source
# code file to Nori, be sure to include it in this list.
add_executable(min-ray

  # Header files
  include/min-ray/bbox.h
  include/min-ray/bitmap.h
  include/min-ray/block.h
  include/min-ray/bsdf.h
  include/min-ray/accel.h
  include/min-ray/camera.h
  include/min-ray/color.h
  include/min-ray/common.h
  include/min-ray/dpdf.h
  include/min-ray/frame.h
  include/min-ray/integrator.h
  include/min-ray/emitter.h
  include/min-ray/mesh.h
  include/min-ray/object.h
  include/min-ray/parser.h
  include/min-ray/proplist.h
  include/min-ray/ray.h
  include/min-ray/rfilter.h
  include/min-ray/sampler.h
  include/min-ray/scene.h
  include/min-ray/timer.h
  include/min-ray/transform.h
  include/min-ray/vector.h
  include/min-ray/warp.h
  include/min-ray/rendermode.h

  # Source code files
  src/core/bitmap.cc
  src/core/block.cc
  src/accelerators/accel.cc
  src/core/common.cc
  src/bsdfs/diffuse.cc
  src/core/gui.cc
  src/samplers/independent.cc
  src/main.cc
  src/core/mesh.cc
  src/core/obj.cc
  src/core/object.cc
  src/core/parser.cc
  src/cameras/perspective.cc
  src/core/proplist.cc
  src/core/rfilter.cc
  src/core/scene.cc
  src/core/warp.cc
  src/bsdfs/microfacet.cc
  src/bsdfs/mirror.cc
  src/bsdfs/dielectric.cc
  src/renderers/blockwise.cc
  src/renderers/progressive.cc
  src/integrators/normals.cc
)

add_definitions(${NANOGUI_EXTRA_DEFS})

target_link_libraries(min-ray tbb_static pugixml IlmImf nanogui ${NANOGUI_EXTRA_LIBS})

# vim: set et ts=2 sw=2 ft=cmake nospell: