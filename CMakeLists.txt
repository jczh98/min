cmake_minimum_required(VERSION 3.1.4)

project(min-ray VERSION 0.1.0)

enable_testing()

set(CMAKE_CXX_STANDARD  17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -pthread")	
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

set(EXECUTABLE_NAME min-ray-cli)

add_subdirectory(external/fmt)

file(GLOB MIN_RAY_API
	"include/min-ray/*.h"
	)

file(GLOB MIN_RAY_CORE
	"src/core/accelerators/*.h"
	"src/core/accelerators/*.cc"
	"src/core/bsdfs/*.h"
	"src/core/bsdfs/*.cc"
	"src/core/integrators/*.h"
	"src/core/integrators/*.cc"
	"src/core/*.cc"
	"src/core/lights/*.h"
	"src/core/lights/*.cc"
	"src/core/samplers/*.h"
	"src/core/samplers/*.cc"
	"src/core/cameras/*.h"
	"src/core/cameras/*.cc"
	"src/core/materials/*.h"
	"src/core/materials/*.cc"
	"src/core/importers/*.h"
	"src/core/importers/*.cc"
	"src/core/shaders/*.h"
	"src/core/shaders/*.cc"
	"src/core/shapes/*.h"
	"src/core/shapes/*.cc"
	"external/tinyobjloader/tiny_obj_loader.cc"
	)

add_executable(${EXECUTABLE_NAME} 
	src/standalone/main.cc 
	${MIN_RAY_API}
	${MIN_RAY_CORE}
	)

target_include_directories(${EXECUTABLE_NAME} 
	PUBLIC
	"include"
	external/tinyobjloader
	external/json/include
	external/glm
	external/fmt/include
	)

target_link_libraries(${EXECUTABLE_NAME}
	PUBLIC 
	fmt
	stdc++fs
)

add_executable(test_importer tests/test_importer.cc ${MIN_RAY_API} ${MIN_RAY_CORE})

target_include_directories(test_importer
	PUBLIC
	"include"
	external/tinyobjloader
	external/glm
	external/fmt/include
	external/cereal/include
	external/json/includ
)

target_link_libraries(test_importer
	PUBLIC
	fmt
)

add_executable(test_object tests/test_object.cc ${MIN_RAY_API} ${MIN_RAY_CORE})

target_include_directories(test_object
	PUBLIC
	"include"
	external/tinyobjloader
	external/glm
	external/fmt/include
	external/cereal/include
	external/json/includ
)

target_link_libraries(test_object
	PUBLIC
	fmt
)