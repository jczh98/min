cmake_minimum_required(VERSION 2.8.10)

project(min-ray)

set(CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake/")

if (WIN32)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_ROOT}/Modules")
else ()
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_ROOT}/Modules")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/")
endif ()

find_package(glfw3 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(TBB CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)

add_subdirectory(ext ext_build)
add_subdirectory(external/glad)

include_directories(
        # Nori include files
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        # tinyformat string formatting library
        ${TFM_INCLUDE_DIR}
        # Pseudorandom number generator
        ${PCG32_INCLUDE_DIR}
        # PugiXML parser
        ${PUGIXML_INCLUDE_DIR}
        # Portable filesystem API
        ${FILESYSTEM_INCLUDE_DIR}
        # STB Image Write
        ${STB_IMAGE_WRITE_INCLUDE_DIR}
        # Lodepng
        external/lodepng

        external/imgui
        external/imgui/examples
)

file(GLOB IMGUI_SRC
        external/imgui/examples/imgui_impl_glfw.h
        external/imgui/examples/imgui_impl_opengl3.h
        external/imgui/examples/imgui_impl_glfw.cpp
        external/imgui/examples/imgui_impl_opengl3.cpp
        external/imgui/*.cpp
        external/imgui/*.h
        )

set(MIN_RAY_LIBS
        pugixml glad glfw
        fmt::fmt
        nlohmann_json nlohmann_json::nlohmann_json
        TBB::tbb
        spdlog::spdlog
        Eigen3::Eigen
        )

file(GLOB MIN_RAY_CORE
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
        include/min-ray/preview_gui.h
        include/min-ray/interface.h
        include/min-ray/logging.h
        include/min-ray/json.h

        # Source code files
        src/core/bitmap.cc
        src/core/block.cc
        src/core/interface.cc
        src/core/interfaces.cc
        src/core/logging.cc
        src/accelerators/accel.cc
        src/core/common.cc
        src/bsdfs/diffuse.cc
        src/samplers/independent.cc
        src/main.cc
        src/core/mesh.cc
        src/meshes/obj.cc
        src/core/object.cc
        src/core/preview_gui.cc
        src/core/parser.cc
        src/cameras/perspective.cc
        src/core/proplist.cc
        src/filters/rfilter.cc
        src/core/scene.cc
        src/core/warp.cc
        #src/bsdfs/microfacet.cc
        #src/bsdfs/mirror.cc
        #src/bsdfs/dielectric.cc
        src/renderers/blockwise.cc
        src/renderers/progressive.cc
        src/integrators/normals.cc
        #src/integrators/path.cc
        src/lights/area_light.cc

        external/lodepng/lodepng.cpp
        external/lodepng/lodepng_util.cpp
        )

add_executable(min-ray
        ${MIN_RAY_CORE}
        ${IMGUI_SRC}
        )

SET_PROPERTY(TARGET min-ray PROPERTY CXX_STANDARD 17)
SET_PROPERTY(TARGET min-ray PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(min-ray
        PRIVATE
        ${MIN_RAY_LIBS})