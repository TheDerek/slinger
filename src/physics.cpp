//
// Created by derek on 19/09/20.
//

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>

#include <box2d/box2d.h>
#include <iostream>

#include "physics.h"
#include "misc_components.h"

using Drawable = std::unique_ptr<sf::Shape>;


Physics::Physics() : world_(b2Vec2(0, -98.f)) {}

const float Physics::TIME_STEP = 1 / 60.f;

void Physics::handlePhysics(entt::registry &registry, float delta) {
    world_.Step(TIME_STEP, 6, 18);

    registry.view<Fixture>().each(
            [delta, &registry](const auto entity, Fixture &fixture) {
                b2Body *body = fixture.value->GetBody();

                if (WalkDir *walkDir = registry.try_get<WalkDir>(entity)) {
                    bool walking = walkDir->value != 0;
                    body->SetFixedRotation(!walking);
                    body->ApplyAngularImpulse(-40000 * delta * walkDir->value, false);
                    body->GetFixtureList()->SetFriction(walking ? 0.5 : 5.f);
                }

                if (Drawable *drawable = registry.try_get<Drawable>(entity)) {
                    (*drawable)->setPosition(
                            body->GetPosition().x,
                            body->GetPosition().y
                    );

                    (*drawable)->setRotation((body->GetAngle() * 180.f / 3.145f) + fixture.angleOffset);
                }
            }
    );
}

BodyPtr Physics::makeBody(sf::Vector2f pos, float rot, b2BodyType bodyType) {
    b2BodyDef bodyDef;
    bodyDef.type = bodyType;
    bodyDef.position.Set(pos.x, pos.y);
    bodyDef.angle = rot;
    bodyDef.angularDamping = 0.01f;
    bodyDef.linearDamping = 0.01f;

    return BodyPtr(world_.CreateBody(&bodyDef));
}

Fixture Physics::makeFixture(sf::Shape *shape, entt::registry &reg, entt::entity bodyEntity) {

    const BodyPtr &body = *reg.try_get<BodyPtr>(bodyEntity);
    assert(body);

    auto *rect = dynamic_cast<sf::RectangleShape *>(shape);
    auto *circle = dynamic_cast<sf::CircleShape *>(shape);

    if (!(rect || circle)) {
        throw std::runtime_error("Can only make components from a rectangle and circle so far");
    }

    std::unique_ptr<b2Shape> fixtureShape;

    if (rect) {
        b2PolygonShape box;
        box.SetAsBox(
            rect->getSize().x / 2.f,
            rect->getSize().y / 2.f,
            b2Vec2(
                    shape->getPosition().x,
                    shape->getPosition().y
            ),
            shape->getRotation()
        );

        // Set the origin of the shape for rotations
        shape->setOrigin((rect->getSize() / 2.f) - shape->getPosition());

        fixtureShape = std::make_unique<b2PolygonShape>(box);
    }

    if (circle) {
        b2CircleShape circleShape;
        circleShape.m_radius = circle->getRadius();

        fixtureShape = std::make_unique<b2CircleShape>(circleShape);
        shape->setOrigin(sf::Vector2f(circle->getRadius(), circle->getRadius()));
    }

    b2FixtureDef fixtureDef;
    fixtureDef.shape = fixtureShape.get();
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.9f;

    return Fixture{
        FixturePtr(body->CreateFixture(&fixtureDef)),
        shape->getRotation(),
        shape->getPosition()
    };
}

b2Vec2 Physics::tob2(const sf::Vector2f &vec) {
    return b2Vec2(vec.x, vec.y);
}

float Physics::toRadians(float deg) {
    return deg * Physics::PI / 180.f;
}

float Physics::toDegrees(float deg) {
    return deg * (180.f / Physics::PI);
}

void Physics::createJoint(const b2JointDef &jointDef) {
    world_.CreateJoint(&jointDef);
}

void Physics::walkRight(entt::registry &registry, entt::entity entity) {
    if (!registry.has<BodyPtr>(entity)) {
        throw std::runtime_error("Entity has to have a body in order to move");
    }

    BodyPtr &body = registry.get<BodyPtr>(entity);
    body->ApplyForceToCenter(b2Vec2(100, 0), false);
}

void BodyDeleter::operator()(b2Body *body) const {
    body->GetWorld()->DestroyBody(body);
}

void FixtureDeleter::operator()(b2Fixture *fixture) const {
    fixture->GetBody()->DestroyFixture(fixture);
}