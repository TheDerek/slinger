cmake_minimum_required(VERSION 3.18)

project(slinger)

set(CMAKE_CXX_STANDARD 20)

add_executable(slinger src/main.cpp src/physics.cpp src/physics.h src/illustrator.cpp src/illustrator.h src/input_manager.cpp src/input_manager.h src/misc_components.h src/body_builder.cpp src/body_builder.h)

find_package(box2d REQUIRED)
find_package(SFML 2.5 COMPONENTS graphics audio system window REQUIRED)
find_package(EnTT REQUIRED)

target_link_libraries(slinger sfml-graphics sfml-audio sfml-system sfml-window ${EnTTTargets} box2d)