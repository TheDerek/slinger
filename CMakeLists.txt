cmake_minimum_required(VERSION 3.17)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
  CACHE STRING "Vcpkg toolchain file")

project(slinger)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)
