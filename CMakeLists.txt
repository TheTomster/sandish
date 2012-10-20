cmake_minimum_required(VERSION 2.8)

# find_package(OpenGL)

include_directories(tgl gl3w)
add_subdirectory(tgl)
add_subdirectory(gl3w)

project(sandish)

add_executable(sandish
  board.c
  cam.c
  cursor.c
  draw.c
  list.c
  lualink.c
  main.c
  matrix.c
  panic.c
  registry.c
  screen.c
  tick.c
  vec4.c
)

target_link_libraries(sandish tgl lua5.2 glfw gl3w GL)

set(shader_files
  geometry.glsl
  vertex.glsl
  util.frag.glsl

  concrete.frag.glsl
  man.frag.glsl
  sand.frag.glsl
  sandstone.frag.glsl
  water.frag.glsl

  ssao/fragment.glsl
  ssao/vertex.glsl
)

set(lua_files
  main.lua
  utils.lua

  concrete.lua
  man.lua
  sand.lua
  sandstone.lua
  water.lua
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
