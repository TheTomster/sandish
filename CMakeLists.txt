cmake_minimum_required(VERSION 2.8)

find_package(OpenGL)

project(sandish)

add_executable(sandish
  main.c board.c cam.c draw.c utils.c screen.c)
target_link_libraries(sandish lua glfw gl3w ${OPENGL_LIBRARY})

set(shader_files
  geometry.glsl
  vertex.glsl
  fragment.glsl
)

foreach(shader ${shader_files})
  add_custom_command(OUTPUT "res/${shader}"
    COMMAND cmake -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/res/${shader}"
        "${CMAKE_CURRENT_BINARY_DIR}/res/${shader}"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/res/${shader}"
    COMMENT "Copying shader ${shader}")
  list(APPEND shader_list "res/${shader}")
endforeach(shader)

add_custom_target(shaders ALL DEPENDS ${shader_list})
