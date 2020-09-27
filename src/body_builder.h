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

class BodyBuilder;

struct ShapePrototype {
    std::unique_ptr<sf::Shape> shape;
    bool draw;
    bool makeFixture;
    bool sensor;
    float density;
    float friction;
};


class ShapeBuilder {
private:
    BodyBuilder& bodyBuilder_;
public:
    ShapeBuilder(BodyBuilder& bodyBuilder): bodyBuilder_(bodyBuilder)
    {
    }

    BodyBuilder& create() {
        return bodyBuilder_;
    }
};

class BodyBuilder {
private:
    entt::entity entity_;
    entt::registry &registry_;
    Physics &physics_;
    std::vector<ShapePrototype> shapes_;
    sf::Vector2f pos_;
    b2BodyType bodyType_ = b2_dynamicBody;
    float rot_ = 0;
    bool fixedRotation_ = false;

public:
    explicit BodyBuilder(entt::registry &registry, Physics &physics) :
        registry_(registry),
        physics_(physics) {
        entity_ = registry.create();
    }

    BodyBuilder &addRect(
        float width,
        float height,
        float rot = 0,
        float x = 0,
        float y = 0,
        bool draw = true,
        bool makeFixture = false,
        bool sensor = false,
        float density = 1,
        float friction = 0
    ) {
        if (!draw && !makeFixture) {
            return *this;
        }

        sf::RectangleShape rect(sf::Vector2f(width, height));
        rect.rotate(rot);
        rect.setPosition(x, y);

        shapes_.push_back(ShapePrototype {
            std::make_unique<sf::RectangleShape>(rect),
            draw,
            makeFixture,
            sensor,
            density,
            friction
        });

        return *this;
    }

    BodyBuilder &setPos(float x, float y) {
        pos_ = sf::Vector2(x, y);
        return *this;
    }

    BodyBuilder &setType(b2BodyType type) {
        bodyType_ = type;
        return *this;
    }

    BodyBuilder &setRot(float rot) {
        rot_ = rot;
        return *this;
    }

    BodyBuilder &setFixedRotation(bool fixedRotation) {
        fixedRotation_ = fixedRotation;
        return *this;
    }

    entt::entity create() {
        auto &body = physics_.makeBody(
            entity_,
            pos_,
            rot_,
            bodyType_
        );

        body->SetFixedRotation(fixedRotation_);

        for (auto& rect : shapes_) {
            auto shapeEntity = registry_.create();

            if (rect.makeFixture) {
                physics_.makeFixture(shapeEntity, rect.shape.get(), registry_, entity_);
            }

            if (rect.draw) {
                registry_.emplace<Drawable>(shapeEntity, std::move(rect.shape));
            }
        }

        return entity_;
    }
};


#endif //SLINGER_BODY_BUILDER_H
