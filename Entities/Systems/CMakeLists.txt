add_library(Systems "RenderSystem.cpp" "MovementSystem.h" "MovementSystem.cpp")

target_include_directories(Systems PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})