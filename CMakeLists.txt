# CMakeList.txt : CMake project for OpenGLTest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)

set(GL_DEP_DIR "C:/deps")

project ("OpenGLTest")

include_directories(${GL_DEP_DIR}/imgui)
include_directories(${GL_DEP_DIR}/imgui/backends)
include_directories(${GL_DEP_DIR}/SDL2/include)
include_directories(${GL_DEP_DIR}/glm)
include_directories(${GL_DEP_DIR}/glad/include)
include_directories(${GL_DEP_DIR}/tinygltf)
include_directories(${GL_DEP_DIR}/tinyobjloader)
include_directories(${GL_DEP_DIR}/nlohmannjson/include)
include_directories(${GL_DEP_DIR}/stb)

link_directories(${GL_DEP_DIR}/SDL2/lib/x64)
link_directories(${GL_DEP_DIR}/glew/lib/Release/x64)

add_subdirectory(Entities)
add_subdirectory(Utilities)

file(GLOB IMGUI_SOURCES ${GL_DEP_DIR}/imgui/*.cpp)
add_library(Imgui ${IMGUI_SOURCES} ${GL_DEP_DIR}/imgui/backends/imgui_impl_opengl3.cpp ${GL_DEP_DIR}/imgui/backends/imgui_impl_sdl.cpp)

# Add source to this project's executable.
add_executable (OpenGLTest "OpenGLTest.cpp" "Engine.cpp")

target_link_libraries(OpenGLTest SDL2 SDL2main glew32 OpenGL32 Imgui Entities Utilities)

add_custom_command(TARGET OpenGLTest POST_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory
						   ${CMAKE_SOURCE_DIR}/Resources
						   ${CMAKE_CURRENT_BINARY_DIR}/Resources)