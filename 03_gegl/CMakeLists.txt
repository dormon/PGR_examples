cmake_minimum_required(VERSION 3.18)

project(PGR_examples_03)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1 ) 

option(SDL_SHARED "" OFF)
option(SDL_STATIC "" ON)
add_subdirectory(../libs/SDL2-2.0.16 sdl2_build)

add_subdirectory(../libs/geGL gegl_build)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} SDL2-static SDL2::SDL2main geGL::geGL)

add_custom_target(run "./${PROJECT_NAME}")
