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
        .setPos(0, 0)
        .setFixedRotation(true)
        .addRect(10, 20)
            .setColor(sf::Color(100, 200, 50))
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
            .setZIndex(1)
            .create()
        .create();

    // Add movement to player
    registry.emplace<Movement>(player);
    registry.emplace<InputComponent>(
        player, InputComponent {
            {sf::Keyboard::Key::A, InputAction::WALK_LEFT},
            {sf::Keyboard::Key::D, InputAction::WALK_RIGHT},
            {JustPressed(sf::Keyboard::Space), Jump { 80 } }
        });

    // The players arm
    auto arm = BodyBuilder(registry, physics)
        .addRect(3, 10)
            .setColor(sf::Color(255, 255, 255))
            .setSensor()
            .makeFixture()
            .draw()
            .setZIndex(1)
            .setDensity(0)
            .setPos(0, 3.5f)
            .create()
        .attach(player, 0, 7, 0, 0)
        .setFixedRotation(true)
        .create();

    // Add arm inputs
    registry.emplace<entt::tag<"rotate_to_mouse"_hs>>(arm);
    registry.emplace<InputComponent>(arm, InputComponent {
        {JustPressed(sf::Mouse::Left), FireRope { sf::Vector2f(0, 5) } }
    });

    // Sort drawable entities by z index
    registry.sort<Drawable>([](const auto &lhs, const auto &rhs) {
        return lhs.zIndex < rhs.zIndex;
    });
}

int main() {
    std::cout << "Hello, World!" << std::endl;

    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
    window.setKeyRepeatEnabled(false);

    entt::registry registry;
    Physics physics(registry);
    Illustrator illustrator(window);
    InputManager inputManager(window, physics.getDispatcher(), registry);

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