add_library(slingerlib
    physics.cpp
    physics.h
    illustrator.cpp
    illustrator.h
    input_manager.cpp
    input_manager.h
    misc_components.h
    body_builder.cpp
    body_builder.h
    events.h
    checkpoint_manager.cpp
    checkpoint_manager.h
    map_maker/regexer.cpp
    map_maker/regexer.h
    map_maker/map_maker.h
    map_maker/map_maker.cpp
    map_maker/path_builder.cpp
    map_maker/path_builder.h
    scenes/scene.h
    scenes/level_scene.cpp
    scenes/level_scene.h
    scenes/main_menu_scene.cpp
    scenes/main_menu_scene.h
    scenes/scene_manager.cpp
    scenes/scene_manager.h
        scenes/tutorial_scene.cpp
        scenes/tutorial_scene.h)

target_include_directories(slingerlib PUBLIC
    .
    map_maker/.
    scenes/.
)

find_package(unofficial-box2d REQUIRED)
find_package(SFML 2.5 COMPONENTS graphics audio system window REQUIRED)
find_package(EnTT REQUIRED)
find_package(pugixml REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLUT REQUIRED)

include(FindOpenGL)

include_directories(${GLUT_INCLUDE_DIRS} )

target_link_libraries(slingerlib PUBLIC
    ${GLUT_LIBRARY}
    sfml-graphics
    sfml-audio
    sfml-system
    sfml-window
    ${EnTTTargets}
    unofficial::box2d::box2d
    pugixml
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

if (${MSVC})
    target_link_libraries(slingerlib PUBLIC
        OpenGL
    )
else()
    target_link_libraries(slingerlib PUBLIC
        OpenGL::OpenGL
        OpenGL::GLX
    )
endif()
