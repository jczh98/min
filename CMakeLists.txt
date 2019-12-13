cmake_minimum_required(VERSION 3.1.4)

project(min-ray VERSION 0.1.0)

enable_testing()

set(CMAKE_CXX_STANDARD  17)

set(EXECUTABLE_NAME min-ray-cli)

add_subdirectory(external/fmt)

file(GLOB MIN_RAY_API
	"include/min-ray/*.h"
	"include/min.ray/*.h"
	)

file(GLOB MIN_RAY_CORE
	"src/core/integrators/*.cc"
	"src/core/*.cc"
	"src/core/samplers/*.h"
	"src/core/cameras/*.h"
	)

add_executable(${EXECUTABLE_NAME} 
	src/standalone/main.cc 
	${MIN_RAY_API}
	${MIN_RAY_CORE}
	)

add_executable(min_test tests/test.cc)

target_include_directories(min_test
	PUBLIC
	external/cereal/include
	external/json/include
)

target_include_directories(${EXECUTABLE_NAME} 
	PUBLIC
	"include"
	external/cereal/include
	external/json/include
	external/glm
	external/fmt/include
	)
