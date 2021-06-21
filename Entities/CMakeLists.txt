add_subdirectory(Systems)

add_library(Entities "Models.cpp" "Physics.cpp" "Entities.cpp")

target_include_directories(Entities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Entities Systems)