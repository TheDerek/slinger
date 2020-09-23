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

struct FixtureInfo
{
    FixturePtr value;
    float angleOffset;
    sf::Vector2f posOffset;
    int numberOfContacts = 0;
};

using FixtureInfoPtr = std::shared_ptr<FixtureInfo>;

struct FootSensor {
    FixtureInfoPtr fixture;
};


class ContactListener : public b2ContactListener
{
    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;
};

class Physics
{
    b2World world_;
    entt::registry& registry_;
    static const float TIME_STEP;

public:
    static constexpr float PI=3.14159265358979f;
    static b2Vec2 tob2(const sf::Vector2f& vec);
    static float toRadians(float deg);
    static float toDegrees(float rad);

    explicit Physics(entt::registry&);

    void handlePhysics(entt::registry &registry, float delta);

    BodyPtr makeBody(sf::Vector2f pos, float rot = 0, b2BodyType = b2_dynamicBody);
    FixtureInfoPtr makeFixture(sf::Shape* shape,  entt::registry& reg, entt::entity body);
    void createJoint(const b2JointDef&);

private:
    void manageWalking(entt::entity entity, b2Body &body, Movement &walkDir);
};


#endif //SLINGER_PHYSICS_H
