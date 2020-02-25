cmake_minimum_required(VERSION 2.8.10)

project(min)

enable_testing()

set(CMAKE_CXX_STANDARD  17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake/")

if (MSVC)
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/lib)
    set(CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /std:c++17 /MP /D \"_CRT_SECURE_NO_WARNINGS\" /D \"_ENABLE_EXTENDED_ALIGNED_STORAGE\"")
else()
    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        message("Clang compiler detected. Using std=c++17.")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fsized-deallocation")
    else()
        message(FATAL_ERROR "clang-6/7 is the only supported compiler for Taichi compiler development")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ")
endif ()

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
find_package(spdlog CONFIG REQUIRED)
find_library(lodepng lodepng REQUIRED)
find_path(lodepng_INCLUDES lodepng.h)
find_package(GTest CONFIG REQUIRED)
find_package(TBB CONFIG REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)

add_subdirectory(external/glad)

include_directories(
        ${CMAKE_SOURCE_DIR}
        external/imgui
        external/imgui/examples
        ${lodepng_INCLUDES}
        ${Stb_INCLUDE_DIR}
)

file(GLOB IMGUI_SRC
        external/imgui/examples/imgui_impl_glfw.h
        external/imgui/examples/imgui_impl_opengl3.h
        external/imgui/examples/imgui_impl_glfw.cpp
        external/imgui/examples/imgui_impl_opengl3.cpp
        external/imgui/*.cpp
        external/imgui/*.h
        )

set(MIN_LIBS
        glad glfw
        fmt::fmt
        nlohmann_json nlohmann_json::nlohmann_json
        spdlog::spdlog
        ${lodepng}
        GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main
        TBB::tbb
        tinyobjloader::tinyobjloader
        )

file(GLOB MIN_CORE
        min/*/*.h
        min/*/*.cc
        )

add_executable(min
        ${MIN_CORE}
        ${IMGUI_SRC}
        min/main.cc
        )

target_link_libraries(min
        PRIVATE
        ${MIN_LIBS})

# Tests
add_executable(tests min/tests.cc)
add_test(tests tests)
target_link_libraries(tests
        PRIVATE
        ${MIN_LIBS})