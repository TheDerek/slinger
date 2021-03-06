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
#include "events.h"

struct BodyDeleter {
    void operator()(b2Body *body) const;
};

struct FixtureDeleter {
    void operator()(b2Fixture *fixture) const;
};

struct JointDeleter {
    void operator()(b2Joint *joint) const;
};

using BodyPtr = std::unique_ptr<b2Body, BodyDeleter>;
using FixturePtr = std::unique_ptr<b2Fixture, FixtureDeleter>;
using JointPtr = std::unique_ptr<b2Joint, JointDeleter>;

struct FixtureInfo {
    FixturePtr value;
    float angleOffset;
    sf::Vector2f posOffset;
    entt::entity entity;
    entt::entity bodyEntity;
    int numberOfContacts = 0;
};

using FixtureInfoPtr = std::shared_ptr<FixtureInfo>;

struct FootSensor {
    FixtureInfoPtr fixture;
};


class ContactListener : public b2ContactListener {
    entt::registry& registry_;
    entt::dispatcher &dispatcher_;

public:
    ContactListener(entt::registry &registry, entt::dispatcher &dispatcher);

private:
    void BeginContact(b2Contact *contact) override;
    void EndContact(b2Contact *contact) override;
};

class RopeHitCallback : public b2RayCastCallback {
    entt::registry& registry_;
    b2World& world_;
    entt::entity entity_;
    sf::Vector2f localFireLoc_;

    float ReportFixture(
        b2Fixture *fixture,
        const b2Vec2 &point,
        const b2Vec2 &normal,
        float fraction
    ) override;

public:
    RopeHitCallback(entt::registry &registry, entt::entity entity, b2World &world,
        sf::Vector2f localFireLoc);
};

class Physics {
private:
    b2World world_;
    entt::registry &registry_;
    entt::dispatcher &dispatcher_;

public:
    static const float TIME_STEP;
    static constexpr float PI = 3.14159265358979f;

    explicit Physics(entt::registry &, entt::dispatcher &);

    static b2Vec2 tob2(const sf::Vector2f &vec);
    static float toRadians(float deg);
    static float toDegrees(float rad);
    b2World &getWorld();
    void handlePhysics(entt::registry &registry, float delta, const sf::Vector2f &mousePos);
    BodyPtr makeBody(sf::Vector2f pos, float rot = 0, b2BodyType = b2_dynamicBody);
    BodyPtr &makeBody(entt::entity entity, sf::Vector2f pos, float rot = 0, b2BodyType = b2_dynamicBody);
    FixtureInfoPtr& makeFixture(entt::entity, sf::Shape*, entt::registry&, entt::entity body);

private:
    void manageMovement(entt::entity entity, b2Body &body, Movement &movement);
    void rotateToPoint(b2Body &body, const sf::Vector2f &mousePos);
    bool isOnFloor(entt::entity entity);

    // Event handlers
    void fireRope(Event<FireRope> event);
    void jump(Event<Jump> event);
    void teleport(Event<Teleport> event);
    void onDeath(Event<Death> event);

};

#endif //SLINGER_PHYSICS_H
