#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include <iostream>
#include <memory>

#include <spdlog/spdlog.h>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>

#include "physics.h"
#include "illustrator.h"
#include "input_manager.h"
#include "map_maker.h"
#include "checkpoint_manager.h"

std::string getMap(int argc, char *argv[]) {
    if (argc < 2) {
        return "data/map.svg";
    }

    if (argc > 3) {
        throw std::runtime_error("Incorrect number of arguments supplied");
    }

    return argv[2];
}

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("%Y-%m-%d %@ %! [%l] %v");

    auto mapPath = getMap(argc, argv);
    SPDLOG_INFO("Starting game, using map: {}", mapPath);

    auto settings = sf::ContextSettings();
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(
        sf::VideoMode(1000, 800),
        "Slinger DEV build"
    );

    window.setKeyRepeatEnabled(false);

    entt::registry registry;
    entt::dispatcher dispatcher;

    Physics physics(registry, dispatcher);
    Illustrator illustrator(window, registry, dispatcher);
    InputManager inputManager(window, dispatcher, registry);
    MapMaker mapMaker(registry, physics);
    CheckpointManager checkpointManager(registry, dispatcher);

    mapMaker.make(mapPath);

    sf::Clock deltaClock;
    UIAction action;

    // Start the game loop
    do {
        action = inputManager.handleInput();

        // Clear screen
        window.clear(sf::Color::White);

        // Get the mouse pos
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        auto delta = deltaClock.restart();
        checkpointManager.update(delta);
        physics.handlePhysics(registry, delta.asSeconds(), mousePos);
        illustrator.draw(registry);

        // Update the window
        window.display();
    } while (action != UIAction::CLOSE_GAME);
}