#include <iostream>
#include <memory>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>
#include "physics.h"
#include "illustrator.h"
#include "input_manager.h"
#include "misc_components.h"

sf::Color WHITE = sf::Color(255, 255, 255);

struct Position {
    sf::Vector2f value;
};

void create(entt::registry &registry, Physics& physics) {
    // The floor
    {
        auto entity = registry.create();
        registry.emplace<Position>(entity, sf::Vector2f(10, 10));

        auto rect = std::make_unique<sf::RectangleShape>(sf::RectangleShape(sf::Vector2f(200, 5)));
        rect->setFillColor(sf::Color(255, 100, 50));

        registry.emplace<BodyPtr>(
                entity,
                physics.makeBody(
                        sf::Vector2f(-10, -30),
                        0,
                        b2_staticBody));
        registry.emplace<Fixture>(entity, physics.makeFixture(rect.get(), registry, entity));
        registry.emplace<Drawable>(entity, std::move(rect));
    }


    // The roof
    auto roof = registry.create();
    {
        auto entity = roof;
        registry.emplace<Position>(entity, sf::Vector2f(10, 10));

        auto rect = std::make_unique<sf::RectangleShape>(sf::RectangleShape(sf::Vector2f(120, 4)));
        rect->setFillColor(sf::Color(255, 100, 50));

        registry.emplace<BodyPtr>(
                entity,
                physics.makeBody(
                        sf::Vector2f(0, 30),
                        0,
                        b2_staticBody));
        registry.emplace<Fixture>(entity, physics.makeFixture(rect.get(), registry, entity));
        registry.emplace<Drawable>(entity, std::move(rect));
    }

    // The rectangle
    auto player = registry.create();
    {
        registry.emplace<entt::tag<"player"_hs>>(player);

        auto rect = std::make_unique<sf::RectangleShape>(sf::RectangleShape(sf::Vector2f(10, 20)));
        rect->setFillColor(sf::Color(100, 250, 50));

        auto& body = registry.emplace<BodyPtr>(player, physics.makeBody(sf::Vector2f(-20, 0)));
        body->SetFixedRotation(true);

        registry.emplace<Fixture>(player, physics.makeFixture(rect.get(), registry, player));
        registry.emplace<Drawable>(player, std::move(rect));

        // Add input to this guy
        registry.emplace<Movement>(player, Movement{});
        registry.emplace<InputComponent>(player, InputComponent {
            {sf::Keyboard::Key::A, InputAction::WALK_LEFT},
            {sf::Keyboard::Key::D, InputAction::WALK_RIGHT},
            {sf::Keyboard::Key::Space, InputAction::JUMP}
        });
    }
}

int main() {
    std::cout << "Hello, World!" << std::endl;

    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
    window.setKeyRepeatEnabled(false);

    entt::registry registry;
    Physics physics;
    Illustrator illustrator(window);
    InputManager inputManager(window);

    create(registry, physics);

    sf::Clock deltaClock;
    UIAction action = UIAction::NO_ACTION;

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