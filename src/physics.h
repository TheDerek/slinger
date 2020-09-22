//
// Created by derek on 19/09/20.
//

#ifndef SLINGER_PHYSICS_H
#define SLINGER_PHYSICS_H


#include <entt/entt.hpp>
#include <memory>

#include <box2d/box2d.h>
#include <SFML/Graphics/Shape.hpp>
#include "misc_components.h"

struct BodyDeleter
{
    void operator()(b2Body* body) const;
};

struct FixtureDeleter
{
    void operator()(b2Fixture* fixture) const;
};

using BodyPtr = std::unique_ptr<b2Body, BodyDeleter>;
using FixturePtr = std::unique_ptr<b2Fixture, FixtureDeleter>;

struct Fixture
{
    FixturePtr value;
    float angleOffset;
    sf::Vector2f posOffset;
};

class Physics
{
    b2World world_;
    static const float TIME_STEP;

public:
    static constexpr float PI=3.14159265358979f;
    static b2Vec2 tob2(const sf::Vector2f& vec);
    static float toRadians(float deg);
    static float toDegrees(float rad);

    Physics();

    void handlePhysics(entt::registry &registry, float delta);

    BodyPtr makeBody(sf::Vector2f pos, float rot = 0, b2BodyType = b2_dynamicBody);
    Fixture makeFixture(sf::Shape* shape,  entt::registry& reg, entt::entity body);
    void createJoint(const b2JointDef&);

private:
    void manageWalking(b2Body& body, Movement &walkDir);
};


#endif //SLINGER_PHYSICS_H
