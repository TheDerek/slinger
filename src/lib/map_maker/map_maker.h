//
// Created by derek on 17/10/20.
//

#ifndef SLINGER_MAP_MAKER_H
#define SLINGER_MAP_MAKER_H

#include <entt/entity/registry.hpp>
#include <variant>
#include <regex>

#include "physics.h"
#include "pugixml.hpp"
#include "body_builder.h"

struct Dimensions {
    float x;
    float y;
    float width;
    float height;

    Dimensions(const pugi::xml_node &node);
};


/**
 * Builds Box2d bodies from svg shapes
 */
class MapShapeBuilder {
    entt::registry& registry_;
    Physics& physics_;

public:
    MapShapeBuilder(entt::registry& registry, Physics& physics);

    void makePlayer(const pugi::xml_node &node);
    void makeWall(const pugi::xml_node& node);

    entt::entity makeDeathZone(const pugi::xml_node &node);
    void makeCheckpoint(const pugi::xml_node &node);

private:
    entt::entity makeRect(const pugi::xml_node& node);
    entt::entity makePolygon(const pugi::xml_node &node);

};

/**
 * Builds a level from an svg
 */
class MapMaker {
    MapShapeBuilder mapShapeBuilder_;

public:
    MapMaker(entt::registry& registry, Physics& physics);
    void make(const std::string& path);
};

#endif //SLINGER_MAP_MAKER_H
