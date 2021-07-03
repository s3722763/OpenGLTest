add_subdirectory(RenderSystems)

add_library(Systems "RenderSystem.cpp" "MovementSystem.cpp")

target_include_directories(Systems PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Systems RenderSystems)