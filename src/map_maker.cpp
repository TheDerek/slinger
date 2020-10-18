#include <iostream>

#include "pugixml.hpp"

#include "map_maker.h"
#include "body_builder.h"
#include "input_manager.h"

Dimensions::Dimensions(const pugi::xml_node &node) {
    width = node.attribute("width").as_float();
    height = node.attribute("height").as_float();
    x = node.attribute("x").as_float();
    y = node.attribute("y").as_float() * -1.f;
    x += width/2.f;
    y -= height/2.f;
}

MapMaker::MapMaker(entt::registry &registry, Physics &physics): mapShapeBuilder_(MapShapeBuilder(registry, physics))
{

}

void MapMaker::make(const std::string& path)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(path.c_str());

    if (!result) {
        throw std::runtime_error("could not find file: " + path);
    }

    auto walls = doc.select_nodes("/svg/g[@inkscape:label='walls']/rect");
    std::cout << "Number of walls: " << walls.size() << std::endl;
    for (const auto& wall: walls) {
        mapShapeBuilder_.makeRect(wall.node());
    }

    auto deathZones = doc.select_nodes("/svg/g[@inkscape:label='death_zones']/rect");
    std::cout << "Number of death zones: " << deathZones.size() << std::endl;
    for (const auto& zone: deathZones) {
        mapShapeBuilder_.makeDeathZone(zone.node());
    }

    auto playerNode = doc.select_node("/svg/g[@inkscape:label='objects']/rect[@id='player']");
    if (!playerNode) {
        throw std::runtime_error("Could not find player in svg " + path);
    }

    mapShapeBuilder_.makePlayer(playerNode.node());
}

void MapShapeBuilder::makeRect(const pugi::xml_node& node) {
    sf::Color RED = sf::Color(255, 100, 50);
    Dimensions dimensions(node);

    BodyBuilder(registry_, physics_)
        .setPos(dimensions.x, dimensions.y)
        .setType(b2_staticBody)
        .addRect(dimensions.width, dimensions.height)
            .setColor(RED)
            .draw()
            .makeFixture()
            .setZIndex(0)
            .create()
        .create();
}

MapShapeBuilder::MapShapeBuilder(entt::registry &registry, Physics &physics):
    registry_(registry),
    physics_(physics)
{

}

void MapShapeBuilder::makePlayer(const pugi::xml_node &node) {
    Dimensions dimensions(node);

    // The rectangle player
    auto player = BodyBuilder(registry_, physics_)
        .setPos(dimensions.x, dimensions.y)
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
    registry_.emplace<Follow>(player);

    // Add movement to player
    registry_.emplace<Movement>(player);
    registry_.emplace<InputComponent>(
        player, InputComponent {
            {sf::Keyboard::Key::A, InputAction::WALK_LEFT},
            {sf::Keyboard::Key::D, InputAction::WALK_RIGHT},
            {JustPressed(sf::Keyboard::Space), Jump { 8 } }
        });

    // The players arm
    auto arm = BodyBuilder(registry_, physics_)
        .setPos(dimensions.x, dimensions.y)
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
    registry_.emplace<entt::tag<"rotate_to_mouse"_hs>>(arm);
    registry_.emplace<InputComponent>(arm, InputComponent {
        {
            JustPressed(sf::Mouse::Left), FireRope {
            sf::Vector2f(0, 0.5f),
            sf::Vector2f(0, 0.7f)
        }
        }
    });
}

void MapShapeBuilder::makeDeathZone(const pugi::xml_node &node) {
    Dimensions dimensions(node);

    auto entity = BodyBuilder(registry_, physics_)
        .setPos(dimensions.x, dimensions.y)
        .setType(b2_staticBody)
        .addRect(dimensions.width, dimensions.height)
            .setSensor()
            .makeFixture()
            .setZIndex(3)
            .create()
        .create();

    registry_.emplace<DeathZone>(entity);
}
