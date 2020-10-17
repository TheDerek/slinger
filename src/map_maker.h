//
// Created by derek on 17/10/20.
//

#ifndef SLINGER_MAP_MAKER_H
#define SLINGER_MAP_MAKER_H

#include <entt/entity/registry.hpp>
#include "physics.h"
#include "pugixml.hpp"


/**
 * Builds Box2d bodies from svg shapes
 */
class MapShapeBuilder {
    entt::registry& registry_;
    Physics& physics_;

public:
    MapShapeBuilder(entt::registry& registry, Physics& physics);

    void makePlayer(const pugi::xml_node &node);
    void makeRect(const pugi::xml_node& node);
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