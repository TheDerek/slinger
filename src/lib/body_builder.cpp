//
// Created by derek on 26/09/20.
//

#include "body_builder.h"

#include <utility>


ShapeBuilder &ShapeBuilder::setPos(float x, float y) {
    prototype_.shape->setPosition(x, y);
    return *this;
}

ShapeBuilder &ShapeBuilder::setRot(float x) {
    prototype_.shape->setRotation(x);
    return *this;
}

ShapeBuilder &ShapeBuilder::draw() {
    prototype_.draw = true;
    return *this;
}

ShapeBuilder &ShapeBuilder::makeFixture() {
    prototype_.makeFixture = true;
    return *this;
}

ShapeBuilder &ShapeBuilder::setDensity(float density) {
    prototype_.density = density;
    return *this;
}

ShapeBuilder &ShapeBuilder::setFriction(float friction) {
    prototype_.friction = friction;
    return *this;
}

BodyBuilder &ShapeBuilder::attachToBody() {
    return bodyBuilder_->addShape(std::move(prototype_));
}

ShapeBuilder &ShapeBuilder::setSensor() {
    prototype_.sensor = true;
    return *this;
}

ShapeBuilder &ShapeBuilder::setColor(sf::Color color) {
    prototype_.shape->setFillColor(color);
    return *this;
}

ShapeBuilder &ShapeBuilder::setFootSensor() {
    prototype_.footSensor = true;
    return *this;
}

ShapeBuilder &ShapeBuilder::setZIndex(int z) {
    prototype_.zIndex = z;

    return *this;
}

ShapeBuilder::ShapeBuilder(std::unique_ptr<sf::Shape> shape) {
    prototype_.shape = std::move(shape);
}

entt::entity ShapeBuilder::create(entt::registry &registry, std::optional<entt::entity> entity) {
    if (!entity) {
        entity = registry.create();
    }

    registry.emplace<Drawable>(
        entity.value(),
        Drawable { std::move(prototype_.shape), prototype_.zIndex }
    );

    return entity.value();
}

void ShapeBuilder::setBodyBuilder(BodyBuilder *bodyBuilder) {
    bodyBuilder_ = bodyBuilder;
}

ShapeBuilder ShapeBuilder::CreateRect(float width, float height) {
    auto rect = std::make_unique<sf::RectangleShape>(sf::RectangleShape((sf::Vector2(width, height))));

    return ShapeBuilder(std::move(rect));
}

ShapeBuilder ShapeBuilder::CreatePolygon(const std::vector<sf::Vector2f>& points) {
    auto shape = std::make_unique<sf::ConvexShape>();

    shape->setPointCount(points.size());
    for (std::size_t i = 0; i != points.size(); ++i) {
        shape->setPoint(i, points[i]);
    }

    return ShapeBuilder(std::move(shape));
}


BodyBuilder::BodyBuilder(entt::registry &registry, Physics &physics) :
    registry_(registry),
    physics_(physics) {
    entity_ = registry.create();
}

ShapeBuilder BodyBuilder::addRect(float width, float height) {
    auto builder = ShapeBuilder::CreateRect(width, height);
    builder.setBodyBuilder(this);

    return builder;
}

ShapeBuilder BodyBuilder::addPolygon(const std::vector<sf::Vector2f>& points) {
    auto builder = ShapeBuilder::CreatePolygon(points);
    builder.setBodyBuilder(this);

    return builder;
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
            registry_.emplace<Drawable>(
                shapeEntity,
                Drawable { std::move(prototype.shape), prototype.zIndex }
            );
        }
    }

    if (attachPrototype_.has_value()) {
        b2RevoluteJointDef def;
        def.localAnchorA = b2Vec2(attachPrototype_.value().localAnchorAX, attachPrototype_.value().localAnchorAY);
        def.localAnchorB = b2Vec2(attachPrototype_.value().localAnchorBX, attachPrototype_.value().localAnchorBY);
        def.bodyA = registry_.get<BodyPtr>(attachPrototype_->body).get();
        def.bodyB = body.get();
        def.collideConnected = false;
        def.enableLimit = false;
        physics_.getWorld().CreateJoint(&def);

        registry_.get_or_emplace<Attachments>(entity_).entities.emplace(attachPrototype_->body);
        registry_.get_or_emplace<Attachments>(attachPrototype_->body).entities.emplace(entity_);
    }

    return entity_;
}

BodyBuilder &BodyBuilder::addShape(ShapePrototype prototype) {
    shapes_.push_back(std::move(prototype));
    return *this;
}

BodyBuilder &BodyBuilder::attach(entt::entity body,
    float localAnchorAX, float localAnchorAY,
    float localAnchorBX, float localAnchorBY) {
    attachPrototype_ = AttachPrototype {
      body,
      localAnchorAX,
      localAnchorAY,
      localAnchorBX,
      localAnchorBY
    };

    return *this;
}

