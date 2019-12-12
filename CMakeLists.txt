cmake_minimum_required(VERSION 3.1.4)

project(min-ray VERSION 0.1.0)

enable_testing()

set(CMAKE_CXX_STANDARD  17)

set(EXECUTABLE_NAME min-ray-cli)

add_subdirectory(external/fmt)

file(GLOB MIN_RAY_API
	"include/min-ray/*.h"
	)

file(GLOB MIN_RAY_CORE
	"src/core/integrators/*.cc"
	)
add_executable(${EXECUTABLE_NAME} src/standalone/main.cc ${MIN_RAY_API})

target_include_directories(${EXECUTABLE_NAME} 
	PUBLIC
	external/eigen
	external/fmt/include
	)
