cmake_minimum_required(VERSION 3.19)

project(tomboy)

include(FetchContent)

FetchContent_Declare(
  SDL
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-3.2.8)

FetchContent_MakeAvailable(SDL)

add_executable(tomboy "src/main.cpp")
target_link_libraries(tomboy SDL3::SDL3)
