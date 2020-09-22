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


Physics::Physics() : world_(b2Vec2(0, -200.f)) {}

const float Physics::TIME_STEP = 1 / 60.f;

void Physics::handlePhysics(entt::registry &registry, float delta) {
    world_.Step(TIME_STEP, 6, 18);

    registry.view<Fixture>().each(
            [delta, &registry, this](const auto entity, Fixture &fixture) {
                b2Body *body = fixture.value->GetBody();

                if (Movement *walkDir = registry.try_get<Movement>(entity)) {
                    this->manageWalking(*body, *walkDir);
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

void Physics::manageWalking(b2Body &body, Movement& walkDir) {
    // TODO if not floor skip this

    float vel = body.GetLinearVelocity().x;
    float desiredVel = 0;

    if (walkDir.direction == 0) {
        desiredVel = vel * walkDir.deceleration;
    }

    if (walkDir.direction > 0) {
        desiredVel = b2Min(vel + walkDir.acceleration, walkDir.vel());
    }

    if (walkDir.direction < 0) {
        desiredVel = b2Max(vel - walkDir.acceleration, walkDir.vel());
    }

    float velChange = desiredVel - body.GetLinearVelocity().x;
    float impulse = body.GetMass() * velChange;
    body.ApplyLinearImpulseToCenter(b2Vec2(impulse, 0), false);

    if (walkDir.jumping) {
        std::cout << "Gonna jump!" << std::endl;
        float impulse = body.GetMass() * walkDir.jumpVel;
        body.ApplyLinearImpulseToCenter(b2Vec2(0, impulse), false);
    }
}

void BodyDeleter::operator()(b2Body *body) const {
    body->GetWorld()->DestroyBody(body);
}

void FixtureDeleter::operator()(b2Fixture *fixture) const {
    fixture->GetBody()->DestroyFixture(fixture);
}