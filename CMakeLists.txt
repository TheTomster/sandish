cmake_minimum_required(VERSION 2.8)

find_package(OpenGL)

project(sandish)

add_executable(sandish
  board.c
  cam.c
  draw.c
  list.c
  lualink.c
  main.c
  registry.c
  screen.c
  tick.c
  utils.c
)

target_link_libraries(sandish lua glfw gl3w ${OPENGL_LIBRARY})

set(shader_files
  debug.frag.glsl

  geometry.glsl
  vertex.glsl
)

set(lua_files
  main.lua

  debug.lua
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

foreach(script ${lua_files})
  add_custom_command(OUTPUT "lua/${script}"
    COMMAND cmake -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/lua/${script}"
        "${CMAKE_CURRENT_BINARY_DIR}/lua/${script}"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/lua/${script}"
    COMMENT "Copying script ${script}")
  list(APPEND script_list "lua/${script}")
endforeach(script)

add_custom_target(scripts ALL DEPENDS ${script_list})
