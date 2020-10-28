#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include <iostream>
#include <regex>

#include <spdlog/spdlog.h>
#include <pugixml.hpp>

#include "map_maker.h"
#include "body_builder.h"
#include "input_manager.h"
#include "path_builder.h"

const sf::Color MapShapeBuilder::WALL_COLOUR = sf::Color(50, 50, 50); // sf::Color(255, 100, 50);
const sf::Color MapShapeBuilder::DECORATION_COLOUR = sf::Color(200, 200, 200);

const int MapShapeBuilder::BASE_Z_INDEX = 0;
const int MapShapeBuilder::WALL_Z_INDEX = 0;
const int MapShapeBuilder::DECORATION_Z_INDEX = 1;
const int MapShapeBuilder::PLAYER_BODY_Z_INDEX = 2;
const int MapShapeBuilder::PLAYER_ARM_Z_INDEX = 3;

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

    auto walls = doc.select_nodes("/svg/g[@inkscape:label='walls']/*");
    for (const auto& wall: walls) {
        mapShapeBuilder_.makeWall(wall.node());
    }
    SPDLOG_DEBUG("Added {} walls from {}", walls.size(), path);

    auto deathZones = doc.select_nodes("/svg/g[@inkscape:label='death_zones']/*");
    SPDLOG_DEBUG("Added {} death zones from {}", deathZones.size(), path);
    for (const auto& zone: deathZones) {
        mapShapeBuilder_.makeDeathZone(zone.node());
    }

    auto checkPoints = doc.select_nodes("/svg/g[@inkscape:label='checkpoints']/rect");
    SPDLOG_DEBUG("Added {} checkpoints from {}", checkPoints.size(), path);
    for (const auto& zone: checkPoints) {
        mapShapeBuilder_.makeCheckpoint(zone.node());
    }

    auto decorations = doc.select_nodes("/svg/g[@inkscape:label='decorations']/*");
    for (const auto& decoration: decorations) {
        mapShapeBuilder_.makeDecoration(decoration.node());
    }

    auto playerNode = doc.select_node("/svg/g[@inkscape:label='objects']/rect[@id='player']");
    if (!playerNode) {
        throw std::runtime_error("Could not find player in svg " + path);
    }

    mapShapeBuilder_.makePlayer(playerNode.node());

    SPDLOG_INFO("Successfully loaded {} as the current level", path);
}

void MapShapeBuilder::makeWall(const pugi::xml_node &node) {
    if (strcmp(node.name(), "rect") == 0) {
        makeRect(node);
    }
    else if (strcmp(node.name(), "path") == 0) {
        makePolygon(node);
    } else {
        throw std::runtime_error("Unsupported element type for wall");
    }
}


MapShapeBuilder::MapShapeBuilder(entt::registry &registry, Physics &physics):
    registry_(registry),
    physics_(physics)
{
    spikeTexture_.loadFromFile("data/spike.png");
    spikeTexture_.setRepeated(true);
    spikeTexture_.setSmooth(true);
}

void MapShapeBuilder::makePlayer(const pugi::xml_node &node) {
    Dimensions dimensions(node);

    // The rectangle player
    auto player = BodyBuilder(registry_, physics_)
        .setPos(dimensions.x, dimensions.y)
        .setFixedRotation(true)
        .addRect(1, 2)
            .setColor(sf::Color(235, 186, 52))
            .setOutline(0.1f)
            .makeFixture()
            .draw()
            .setZIndex(MapShapeBuilder::PLAYER_BODY_Z_INDEX)
            .attachToBody()
        .addRect(0.8f, 0.1f)
            .setPos(0, -1)
            .setSensor()
            .makeFixture()
            .setFootSensor()
            .attachToBody()
        .create();

    // Follow the player, enable checkpoints and add a timer
    registry_.emplace<Follow>(player);
    registry_.emplace<Respawnable>(player, sf::Vector2f(dimensions.x, dimensions.y), sf::seconds(2));
    registry_.emplace<Timeable>(player);

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
            .setColor(sf::Color(235, 186, 52))
            .setOutline(0.1f)
            .setSensor()
            .makeFixture()
            .draw()
            .setZIndex(PLAYER_ARM_Z_INDEX)
            .setDensity(0)
            .setPos(0, 0.4f)
            .attachToBody()
        .attach(player, 0, 0.7f, 0, 0)
        .setFixedRotation(true)
        .create();

    // Add arm inputs
    registry_.emplace<entt::tag<"rotate_to_mouse"_hs>>(arm);
    registry_.emplace<InputComponent>(arm, InputComponent {
        {
            JustPressed(sf::Mouse::Left),
            FireRope
            {
                sf::Vector2f(0, 0.5f),
                sf::Vector2f(0, 0.7f)
            }
        }
    });
}


entt::entity MapShapeBuilder::makeRect(const pugi::xml_node& node) {
    Dimensions dimensions(node);

    return BodyBuilder(registry_, physics_)
        .setPos(dimensions.x, dimensions.y)
        .setType(b2_staticBody)
        .addRect(dimensions.width, dimensions.height)
            .setColor(WALL_COLOUR)
            .draw()
            .makeFixture()
            .setZIndex(WALL_Z_INDEX)
            .attachToBody()
        .create();
}

entt::entity MapShapeBuilder::makePolygon(const pugi::xml_node &node) {
    auto svgPoints = node.attribute("d").as_string();
    auto points = PathBuilder::build(svgPoints);

    return BodyBuilder(registry_, physics_)
        .setPos(0, 0)
        .setType(b2_staticBody)
            .addPolygon(points)
            .setColor(WALL_COLOUR)
            .draw()
            .setZIndex(WALL_Z_INDEX)
            .makeFixture()
            .attachToBody()
        .create();
}

entt::entity MapShapeBuilder::makeDeathZone(const pugi::xml_node &node) {
    entt::entity entity;

    auto label = node.attribute("inkscape:label");
    bool spikes = strcmp(label.value(), "spikes") == 0;

    if (strcmp(node.name(), "rect") == 0) {
        Dimensions dimensions(node);
        int textureWidth = spikeTexture_.getSize().x * dimensions.width;

        entity = BodyBuilder(registry_, physics_)
            .setPos(dimensions.x, dimensions.y)
            .setType(b2_staticBody)
            .addRect(dimensions.width, dimensions.height)
                .setSensor()
                .draw(spikes)
                .setTexture(&spikeTexture_)
                .setTextureRect(sf::IntRect(1, 1, textureWidth, spikeTexture_.getSize().y))
                .makeFixture()
                .attachToBody()
            .create();

    } else if (strcmp(node.name(), "path") == 0) {
        auto svgPoints = node.attribute("d").as_string();
        auto points = PathBuilder::build(svgPoints);

        entity = BodyBuilder(registry_, physics_)
            .setPos(0, 0)
            .setType(b2_staticBody)
            .addPolygon(points)
                .makeFixture()
                .setSensor()
                .attachToBody()
            .create();
    } else {
        throw std::runtime_error("Unsupported element type for death zone");
    }

    registry_.emplace<DeathZone>(entity, DeathZone {spikes = spikes});
    return entity;
}

void MapShapeBuilder::makeCheckpoint(const pugi::xml_node &node) {
    Dimensions dimensions(node);

    if (strcmp(node.name(), "rect") != 0) {
        throw std::runtime_error("Unsupported element type for checkpoint");
    }

    auto entity = BodyBuilder(registry_, physics_)
        .setPos(dimensions.x, dimensions.y)
        .setType(b2_staticBody)
        .addRect(dimensions.width, dimensions.height)
        .setSensor()
        .makeFixture()
        .attachToBody()
        .create();

    // Respawn at the bottom of the checkpoint with a small jump for the player
    auto respawnLoc = sf::Vector2f(dimensions.x, (dimensions.y - dimensions.height / 2.f) + 2.3f);

    auto label = node.attribute("inkscape:label");
    bool finish = strcmp(label.value(), "finish") == 0;

    registry_.emplace<Checkpoint>(entity, respawnLoc, finish);
}

void MapShapeBuilder::makeDecoration(const pugi::xml_node &node) {
    if (strcmp(node.name(), "rect") == 0) {
        Dimensions dimensions(node);
        ShapeBuilder::CreateRect(dimensions.width, dimensions.height)
            .setPos(dimensions.x - dimensions.width / 2.f, dimensions.y - dimensions.height / 2.f)
            .setColor(DECORATION_COLOUR)
            .setZIndex(DECORATION_Z_INDEX)
            .create(registry_);

    } else if (strcmp(node.name(), "path") == 0) {
        auto svgPoints = node.attribute("d").as_string();
        auto points = PathBuilder::build(svgPoints);

        ShapeBuilder::CreatePolygon(points)
            .setColor(DECORATION_COLOUR)
            .setZIndex(DECORATION_Z_INDEX)
            .create(registry_);
    } else {
        throw std::runtime_error("Unsupported element type for death zone");
    }
}


