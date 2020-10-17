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

void create(entt::registry &registry, Physics &physics) {
//    // The floor
//    BodyBuilder(registry, physics)
//        .setPos(0, -30)
//        .setType(b2_staticBody)
//        .addRect(200, 5)
//            .makeFixture()
//            .draw()
//            .setColor(sf::Color(255, 100, 50))
//            .create()
//        .create();

    // The roof
    BodyBuilder(registry, physics)
        .setPos(0, 3)
        .setType(b2_staticBody)
        .addRect(10, 0.5f)
            .makeFixture()
            .draw()
            .setColor(sf::Color(255, 100, 50))
            .create()
        .create();

    // The rectangle player
    auto player = BodyBuilder(registry, physics)
        .setPos(0, 4)
        .setFixedRotation(true)
        .addRect(1, 2)
            .setColor(sf::Color(100, 200, 50))
            .makeFixture()
            .draw()
            .setZIndex(0)
            .create()
        .addRect(0.8f, 0.1f)
            .setPos(0, -1)
            .setSensor()
            .setColor(sf::Color(255, 255, 255))
            .makeFixture()
            .setFootSensor()
            .draw()
            .setZIndex(1)
            .create()
        .create();

    // Follow the player
    registry.emplace<Follow>(player);

    // Add movement to player
    registry.emplace<Movement>(player);
    registry.emplace<InputComponent>(
        player, InputComponent {
            {sf::Keyboard::Key::A, InputAction::WALK_LEFT},
            {sf::Keyboard::Key::D, InputAction::WALK_RIGHT},
            {JustPressed(sf::Keyboard::Space), Jump { 8 } }
        });

    // The players arm
    auto arm = BodyBuilder(registry, physics)
        .addRect(0.3f, 1)
            .setColor(sf::Color(255, 255, 255))
            .setSensor()
            .makeFixture()
            .draw()
            .setZIndex(1)
            .setDensity(0)
            .setPos(0, 0.4f)
            .create()
        .attach(player, 0, 0.7f, 0, 0)
        .setFixedRotation(true)
        .create();

    // Add arm inputs
    registry.emplace<entt::tag<"rotate_to_mouse"_hs>>(arm);
    registry.emplace<InputComponent>(arm, InputComponent {
        {
            JustPressed(sf::Mouse::Left), FireRope {
                sf::Vector2f(0, 0.5f),
                sf::Vector2f(0, 0.7f)
            }
        }
    });
}

int main() {
    std::cout << "Hello, World!" << std::endl;

    sf::RenderWindow window(sf::VideoMode(1000, 800), "SFML window");
    window.setKeyRepeatEnabled(false);

    entt::registry registry;
    entt::dispatcher dispatcher;
    Physics physics(registry, dispatcher);
    Illustrator illustrator(window, registry, dispatcher);
    InputManager inputManager(window, dispatcher, registry);

    create(registry, physics);

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