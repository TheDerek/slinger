//
// Created by derek on 26/09/20.
//

#include "body_builder.h"

#include <utility>

ShapeBuilder::ShapeBuilder(BodyBuilder *bodyBuilder, std::unique_ptr<sf::Shape> shape):
    bodyBuilder_(bodyBuilder)
{
    prototype.shape = std::move(shape);
}

ShapeBuilder &ShapeBuilder::setPos(float x, float y) {
    prototype.shape->setPosition(x, y);
    return *this;
}

ShapeBuilder &ShapeBuilder::setRot(float x) {
    prototype.shape->setRotation(x);
    return *this;
}

ShapeBuilder &ShapeBuilder::draw() {
    prototype.draw = true;
    return *this;
}

ShapeBuilder &ShapeBuilder::makeFixture() {
    prototype.makeFixture = true;
    return *this;
}

ShapeBuilder &ShapeBuilder::setDensity(float density) {
    prototype.density = density;
    return *this;
}

ShapeBuilder &ShapeBuilder::setFriction(float friction) {
    prototype.friction = friction;
    return *this;
}

BodyBuilder &ShapeBuilder::create() {
    return bodyBuilder_->addShape(std::move(prototype));
}

ShapeBuilder &ShapeBuilder::setSensor() {
    prototype.sensor = true;
    return *this;
}

ShapeBuilder &ShapeBuilder::setColor(sf::Color color) {
    prototype.shape->setFillColor(color);
    return *this;
}

ShapeBuilder &ShapeBuilder::setFootSensor() {
    prototype.footSensor = true;
    return *this;
}

BodyBuilder::BodyBuilder(entt::registry &registry, Physics &physics) :
    registry_(registry),
    physics_(physics) {
    entity_ = registry.create();
}

ShapeBuilder BodyBuilder::addRect(float width, float height) {
    return ShapeBuilder(this, std::make_unique<sf::RectangleShape>(sf::RectangleShape((sf::Vector2(width, height)))));
}

BodyBuilder &BodyBuilder::setPos(float x, float y) {
    pos_ = sf::Vector2(x, y);
    return *this;
}

BodyBuilder &BodyBuilder::setType(b2BodyType type) {
    bodyType_ = type;
    return *this;
}

BodyBuilder &BodyBuilder::setRot(float rot) {
    rot_ = rot;
    return *this;
}

BodyBuilder &BodyBuilder::setFixedRotation(bool fixedRotation) {
    fixedRotation_ = fixedRotation;
    return *this;
}

entt::entity BodyBuilder::create() {
    auto &body = physics_.makeBody(
        entity_,
        pos_,
        rot_,
        bodyType_
    );

    body->SetFixedRotation(fixedRotation_);

    for (auto& prototype : shapes_) {
        auto shapeEntity = registry_.create();

        if (prototype.makeFixture) {
            auto& fix = physics_.makeFixture(shapeEntity, prototype.shape.get(), registry_, entity_);
            fix->value->SetSensor(prototype.sensor);

            if (prototype.footSensor) {
                registry_.emplace<FootSensor>(entity_, FootSensor {fix});
            }
        }

        if (prototype.draw) {
            registry_.emplace<Drawable>(shapeEntity, std::move(prototype.shape));
        }
    }

    return entity_;
}

BodyBuilder &BodyBuilder::addShape(ShapePrototype prototype) {
    shapes_.push_back(std::move(prototype));
    return *this;
}
