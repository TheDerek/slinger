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

        auto rect = std::make_unique<sf::RectangleShape>(sf::RectangleShape(sf::Vector2f(30, 5)));
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

        auto rect = std::make_unique<sf::RectangleShape>(sf::RectangleShape(sf::Vector2f(10, 10)));
        rect->setFillColor(sf::Color(100, 250, 50));

        registry.emplace<BodyPtr>(player, physics.makeBody(sf::Vector2f(-20, 0)));
        registry.emplace<Fixture>(player, physics.makeFixture(rect.get(), registry, player));
        registry.emplace<Drawable>(player, std::move(rect));

        registry.get<BodyPtr>(player)->SetFixedRotation(true);

        // Wheel at the bottom
        auto wheel = registry.create();
        auto circle = std::make_unique<sf::CircleShape>(sf::CircleShape(5));
        circle->setFillColor(sf::Color(100, 50, 50));
        auto* texture = new sf::Texture();
        texture->loadFromFile("data/wheel.png");
        circle->setTexture(texture);
        registry.emplace<BodyPtr>(wheel, physics.makeBody(sf::Vector2f(-20, -5)));
        registry.emplace<Fixture>(wheel, physics.makeFixture(circle.get(), registry, wheel));
        registry.emplace<Drawable>(wheel, std::move(circle));

        // Attach the wheel to the box
        auto jointDef = b2RevoluteJointDef();
        jointDef.bodyA = registry.get<BodyPtr>(player).get();
        jointDef.bodyB = registry.get<BodyPtr>(wheel).get();
        jointDef.localAnchorA = b2Vec2(0, -5.f);
        jointDef.localAnchorB = b2Vec2(0, 0);
        jointDef.collideConnected = false;
        physics.createJoint(jointDef);

        // Add input to this guy
        registry.emplace<WalkDir>(wheel, WalkDir{});
        registry.emplace<InputComponent>(wheel, InputComponent {
            {sf::Keyboard::Key::A, InputAction::WALK_LEFT},
            {sf::Keyboard::Key::D, InputAction::WALK_RIGHT},
            {sf::Keyboard::Key::Space, InputAction::JUMP}
        });
    }

    // Rope hanging from the ceiling
    {
        auto chainLength = 3.f;
        auto lastAnchorPoint = sf::Vector2f(0,27.5 - chainLength/2.f);
        auto jointDef = b2RevoluteJointDef();
        auto lastLink = registry.get<BodyPtr>(roof).get();

        for (int i = 0; i < 16; i++)
        {
            jointDef.bodyA = lastLink;
            lastLink->SetLinearDamping(0.3f);

            auto link = registry.create();
            lastLink = registry.emplace<BodyPtr>(link, physics.makeBody(lastAnchorPoint)).get();

            auto rect = std::make_unique<sf::RectangleShape>(sf::RectangleShape(sf::Vector2f(1, chainLength)));
            registry.emplace<Fixture>(link, physics.makeFixture(rect.get(), registry, link));
            registry.emplace<Drawable>(link, std::move(rect));

            jointDef.bodyB = registry.get<BodyPtr>(link).get();
            jointDef.localAnchorA = b2Vec2(0, -chainLength/2.f);
            jointDef.localAnchorB = b2Vec2(0, chainLength/2.f);
            jointDef.collideConnected = false;
            physics.createJoint(jointDef);

            lastAnchorPoint.y -= chainLength;
        }

        jointDef.bodyA = lastLink;
        jointDef.bodyB = registry.get<BodyPtr>(player).get();
        jointDef.localAnchorA = b2Vec2(0, -chainLength/2.f);
        jointDef.localAnchorB = b2Vec2(0, 5.f);
        jointDef.collideConnected = false;
        physics.createJoint(jointDef);
    }

}

int main() {
    std::cout << "Hello, World!" << std::endl;

    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");

    entt::registry registry;
    Physics physics;
    Illustrator illustrator(window);
    InputManager inputManager(window);

    create(registry, physics);

    sf::Clock deltaClock;

    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear screen
        window.clear(WHITE);

        auto delta = deltaClock.restart();
        inputManager.handleInput(registry);
        physics.handlePhysics(registry, delta.asSeconds());
        illustrator.draw(registry);

        // Update the window
        window.display();
    }
}