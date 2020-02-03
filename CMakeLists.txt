cmake_minimum_required(VERSION 2.8.10)

project(min-ray)

set(CMAKE_CXX_STANDARD  17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
find_library(lodepng lodepng REQUIRED)
find_path(lodepng_INCLUDES lodepng.h)

add_subdirectory(external/glad)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        external/imgui
        external/imgui/examples
        ${lodepng_INCLUDES}
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
        glad glfw
        fmt::fmt
        nlohmann_json nlohmann_json::nlohmann_json
        TBB::tbb
        spdlog::spdlog
        Eigen3::Eigen
        ${lodepng}
        )

file(GLOB MIN_RAY_CORE
        include/min-ray/*.h

        src/accelerators/*.cc
        src/bsdfs/diffuse.cc
        src/cameras/*.cc
        src/core/*.cc
        src/filters/*.cc
        src/integrators/*.cc
        src/meshes/obj.cc
        src/lights/*.cc
        src/renderers/blockwise.cc
        src/samplers/*.cc
        src/main.cc
        )

add_executable(min-ray
        ${MIN_RAY_CORE}
        ${IMGUI_SRC}
        )

target_link_libraries(min-ray
        PRIVATE
        ${MIN_RAY_LIBS})