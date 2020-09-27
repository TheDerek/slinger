//
// Created by derek on 26/09/20.
//

#ifndef SLINGER_BODY_BUILDER_H
#define SLINGER_BODY_BUILDER_H

#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "physics.h"
#include "illustrator.h"

class ShapeBuilder;

struct ShapePrototype {
    std::unique_ptr<sf::Shape> shape;
    bool draw = false;
    bool makeFixture = false;
    bool sensor = false;
    bool footSensor = false;
    float density = 1;
    float friction = 0.2f;
};

class BodyBuilder {
private:
    entt::entity entity_;
    entt::registry &registry_;
    Physics &physics_;
    sf::Vector2f pos_;
    b2BodyType bodyType_ = b2_dynamicBody;
    float rot_ = 0;
    bool fixedRotation_ = false;
    std::vector<ShapePrototype> shapes_;

public:

    explicit BodyBuilder(entt::registry &registry, Physics &physics);
    BodyBuilder& addShape(ShapePrototype);
    ShapeBuilder addRect(float width, float height);
    BodyBuilder &setPos(float x, float y);
    BodyBuilder &setType(b2BodyType type);
    BodyBuilder &setRot(float rot);
    BodyBuilder &setFixedRotation(bool fixedRotation);
    entt::entity create();
};

class ShapeBuilder {
private:
    BodyBuilder* bodyBuilder_;

public:
    ShapePrototype prototype;

    ShapeBuilder(BodyBuilder* bodyBuilder, std::unique_ptr<sf::Shape> shape);

    ShapeBuilder& setPos(float x, float y);
    ShapeBuilder& setRot(float x);
    ShapeBuilder& draw();
    ShapeBuilder& makeFixture();
    ShapeBuilder& setDensity(float density);
    ShapeBuilder& setFriction(float friction);
    ShapeBuilder& setSensor();
    ShapeBuilder& setColor(sf::Color color);
    ShapeBuilder& setFootSensor();
    BodyBuilder& create();
};


#endif //SLINGER_BODY_BUILDER_H
