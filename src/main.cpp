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

sf::Color WHITE = sf::Color(255, 255, 255);

struct Position {
    sf::Vector2f value;
};

void create(entt::registry &registry, Physics &physics) {
    // The floor
    BodyBuilder(registry, physics)
        .setPos(0, -30)
        .setType(b2_staticBody)
        .addRect(200, 5)
            .makeFixture()
            .draw()
            .setColor(sf::Color(255, 100, 50))
            .create()
        .create();

    // The roof
    BodyBuilder(registry, physics)
        .setPos(0, 30)
        .setType(b2_staticBody)
        .addRect(100, 5)
            .makeFixture()
            .draw()
            .setColor(sf::Color(255, 100, 50))
            .create()
        .create();

    // The rectangle player
    auto player = BodyBuilder(registry, physics)
        .setPos(-20, 0)
        .setFixedRotation(true)
        .addRect(10, 20)
            .setColor(sf::Color(100, 250, 50))
            .makeFixture()
            .draw()
            .create()
        .addRect(8, 1)
            .setPos(0, -10)
            .setSensor()
            .setColor(sf::Color(255, 255, 255))
            .makeFixture()
            .setFootSensor()
            .draw()
            .create()
        .create();

    registry.emplace<Movement>(player);
    registry.emplace<InputComponent>(
        player, InputComponent{
            {sf::Keyboard::Key::A, InputAction::WALK_LEFT},
            {sf::Keyboard::Key::D, InputAction::WALK_RIGHT},
            {sf::Keyboard::Key::Space, InputAction::JUMP}
        });
}

int main() {
    std::cout << "Hello, World!" << std::endl;

    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
    window.setKeyRepeatEnabled(false);

    entt::registry registry;
    Physics physics(registry);
    Illustrator illustrator(window);
    InputManager inputManager(window);

    create(registry, physics);

    sf::Clock deltaClock;
    UIAction action;

    // Start the game loop
    do {
        action = inputManager.handleInput(registry, window);

        // Clear screen
        window.clear(WHITE);

        auto delta = deltaClock.restart();
        physics.handlePhysics(registry, delta.asSeconds());
        illustrator.draw(registry);

        // Update the window
        window.display();
    } while (action != UIAction::CLOSE_GAME);
}