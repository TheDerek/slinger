#include <iostream>
#include <memory>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>
#include "physics.h"
#include "illustrator.h"
#include "input_manager.h"
#include "misc_components.h"
#include "body_builder.h"
#include "map_maker.h"

sf::Color WHITE = sf::Color(255, 255, 255);

int main() {
    std::cout << "Hello, World!" << std::endl;

    sf::RenderWindow window(sf::VideoMode(1000, 800), "SFML window");
    window.setKeyRepeatEnabled(false);

    entt::registry registry;
    entt::dispatcher dispatcher;
    Physics physics(registry, dispatcher);
    Illustrator illustrator(window, registry, dispatcher);
    InputManager inputManager(window, dispatcher, registry);
    MapMaker mapMaker(registry, physics);

    mapMaker.make("data/map.svg");

    sf::Clock deltaClock;
    UIAction action;

    // Start the game loop
    do {
        action = inputManager.handleInput();

        // Clear screen
        window.clear(WHITE);

        // Get the mouse pos
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        auto delta = deltaClock.restart();
        physics.handlePhysics(registry, delta.asSeconds(), mousePos);
        illustrator.draw(registry);

        // Update the window
        window.display();
    } while (action != UIAction::CLOSE_GAME);
}
