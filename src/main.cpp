#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include <iostream>
#include <memory>

#include <spdlog/spdlog.h>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>
#include <level_scene.h>

#include "physics.h"
#include "illustrator.h"
#include "input_manager.h"
#include "map_maker.h"
#include "checkpoint_manager.h"

/**
 * Fetch the map from the program arguments, return an empty optional if no map path is
 * provided
 * @param argc
 * @param argv
 * @return the path to the map supplied in the given arguments, or empty if none
 */
std::optional<std::string> getMap(int argc, char *argv[]) {
    if (argc < 2) {
        return std::optional<std::string>();
    }

    if (argc > 3) {
        throw std::runtime_error("Incorrect number of arguments supplied");
    }

    return std::string(argv[1]);
}

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("%Y-%m-%d %@ %! [%l] %v");

    auto mapPath = getMap(argc, argv);
    SPDLOG_INFO("Starting game, using map: {}", mapPath.value_or("No map found"));

    if (!mapPath) {
        throw std::runtime_error("Main menu not yet implemented");
    }

    auto settings = sf::ContextSettings();
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(
        sf::VideoMode(1000, 800),
        "Slinger DEV build"
    );

    window.setKeyRepeatEnabled(false);

    LevelScene level(mapPath.value(), window);

    // Start the game loop
    while (true) {
        level.step();
        window.display();
    }
}