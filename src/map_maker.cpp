#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include <iostream>
#include <regex>

#include <spdlog/spdlog.h>
#include <pugixml.hpp>

#include "map_maker.h"
#include "body_builder.h"
#include "input_manager.h"

const std::regex PathBuilder::PATH_REGEX = std::regex(R"(\s?([^\d])(?:\s(-?\d+\.?\d+)(?:,(-?\d+\.?\d+))?)?)");

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

    auto rectWalls = doc.select_nodes("/svg/g[@inkscape:label='walls']/rect");
    SPDLOG_DEBUG("Added {} rectWalls from {}", rectWalls.size(), path);
    for (const auto& wall: rectWalls) {
        mapShapeBuilder_.makeRect(wall.node());
    }

    auto polyWalls = doc.select_nodes("/svg/g[@inkscape:label='walls']/path");
    SPDLOG_DEBUG("Added {} polyWalls from {}", polyWalls.size(), path);
    for (const auto& wall: polyWalls) {
        mapShapeBuilder_.makePolygon(wall.node());
    }

    auto deathZones = doc.select_nodes("/svg/g[@inkscape:label='death_zones']/rect");
    SPDLOG_DEBUG("Added {} death zones from {}", deathZones.size(), path);
    for (const auto& zone: deathZones) {
        mapShapeBuilder_.makeDeathZone(zone.node());
    }

    auto checkPoints = doc.select_nodes("/svg/g[@inkscape:label='checkpoints']/rect");
    SPDLOG_DEBUG("Added {} checkpoints from {}", checkPoints.size(), path);
    for (const auto& zone: checkPoints) {
        mapShapeBuilder_.makeCheckpoint(zone.node());
    }

    auto playerNode = doc.select_node("/svg/g[@inkscape:label='objects']/rect[@id='player']");
    if (!playerNode) {
        throw std::runtime_error("Could not find player in svg " + path);
    }

    mapShapeBuilder_.makePlayer(playerNode.node());

    SPDLOG_INFO("Successfully loaded {} as the current level", path);
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

    // Follow the player and enable checkpoints
    registry_.emplace<Follow>(player);
    registry_.emplace<Respawnable>(player, sf::Vector2f(dimensions.x, dimensions.y), sf::seconds(2));

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

void MapShapeBuilder::makePolygon(const pugi::xml_node &node) {
    auto svgPoints = node.attribute("d").as_string();
    auto points = PathBuilder::build(svgPoints);

    SPDLOG_INFO("Would make a polygon with the following points: {}", svgPoints);
}

void MapShapeBuilder::makeDeathZone(const pugi::xml_node &node) {
    Dimensions dimensions(node);

    auto entity = BodyBuilder(registry_, physics_)
        .setPos(dimensions.x, dimensions.y)
        .setType(b2_staticBody)
            .addRect(dimensions.width, dimensions.height)
            .setSensor()
            .makeFixture()
//            .setColor(sf::Color(200, 100, 100, 100))
//            .setZIndex(3)
//            .draw()
            .create()
        .create();

    registry_.emplace<DeathZone>(entity);
}

void MapShapeBuilder::makeCheckpoint(const pugi::xml_node &node) {
    Dimensions dimensions(node);

    auto entity = BodyBuilder(registry_, physics_)
        .setPos(dimensions.x, dimensions.y)
        .setType(b2_staticBody)
        .addRect(dimensions.width, dimensions.height)
            .setSensor()
            .makeFixture()
//            .setColor(sf::Color(100, 200, 100, 100))
//            .setZIndex(3)
//            .draw()
            .create()
        .create();

    auto respawnLoc = sf::Vector2f(dimensions.x, (dimensions.y - dimensions.height / 2.f) + 2.3f);
   // auto respawnLoc = sf::Vector2f(dimensions.x, dimensions.y);

    registry_.emplace<Checkpoint>(entity, respawnLoc);
}

PathBuilder::CommandList PathBuilder::getCommands(const std::string &svgPath) {
    auto list = PathBuilder::CommandList();

    std::string searchString = svgPath;
    auto matchResults = std::smatch{};
    while(std::regex_search(searchString, matchResults, PathBuilder::PATH_REGEX))
    {
        if (matchResults.size() < 4) {
            throw std::runtime_error("SVG path string malformed, not enough arguments");
        }

        auto command = PathBuilder::Command(matchResults[1].str()[0]);

        // Default is no arguments
        Arguments args = std::monostate {};

        // Override the empty args if there are arguments
        if (matchResults[3].matched) {
            // There are two arguments, a vector
            args = sf::Vector2f(std::stof(matchResults[2]), std::stof(matchResults[3]));
        } else if (matchResults[2].matched) {
            // A single argument
            args = std::stof(matchResults[2]);
        }

        list.emplace_back(std::make_pair(command, args));

        // Remove the captured string from the path and try again
        searchString = matchResults.suffix();
    }

    return list;
}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

PointList PathBuilder::getPoints(const PathBuilder::CommandList &commands) {
    auto list = PointList();

    for (const auto& [command, arguments] : commands) {
        if (std::holds_alternative<std::monostate>(arguments)) {

        }
    }

    return list;
}

PointList PathBuilder::build(const std::string &string) {
    auto commands = getCommands(string);

    return getPoints(commands);
}
