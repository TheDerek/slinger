//
// Created by derek on 19/09/20.
//

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>

#include <box2d/box2d.h>
#include <iostream>

#include "illustrator.h"
#include "physics.h"
#include "misc_components.h"


Physics::Physics(entt::registry &registry, entt::dispatcher &dispatcher) :
    registry_(registry), dispatcher_(dispatcher),
    world_(b2Vec2(0, -20.f)) {
    world_.SetContactListener(new ContactListener(registry_, dispatcher_));

    dispatcher_.sink<Event<FireRope>>().connect<&Physics::fireRope>(*this);
    dispatcher_.sink<Event<Jump>>().connect<&Physics::jump>(*this);
    dispatcher_.sink<Event<Teleport>>().connect<&Physics::teleport>(*this);
}

const float Physics::TIME_STEP = 1 / 60.f;

void Physics::handlePhysics(entt::registry &registry, float delta, const sf::Vector2f &mousePos) {
    world_.Step(TIME_STEP, 30, 15);

    dispatcher_.update();

    registry.view<FixtureInfoPtr>().each(
        [delta, &registry, this](const auto entity, const FixtureInfoPtr &fixture) {
            b2Body *body = fixture->value->GetBody();

            if (Drawable *drawable = registry.try_get<Drawable>(entity)) {
                (*drawable).value->setPosition(
                    body->GetPosition().x,
                    body->GetPosition().y
                );

                (*drawable).value->setRotation((body->GetAngle() * 180.f / 3.145f) + fixture->angleOffset);
            }
        }
    );

    registry.view<BodyPtr>().each(
        [delta, mousePos, &registry, this](const auto entity, const BodyPtr &body) {
            if (Movement *movement = registry.try_get<Movement>(entity)) {
                this->manageMovement(entity, *body, *movement);
            }

            if (auto* rope = registry.try_get<HoldingRope>(entity)) {
                body->SetFixedRotation(false);
                //this->rotateToPoint(*body, rope->ropeLoc);
            }
            else if (registry.has<entt::tag<"rotate_to_mouse"_hs>>(entity)) {
                body->SetFixedRotation(true);
                this->rotateToPoint(*body, mousePos);
            }

            // Attach a general position to this body so that the camera can follow it
            // without having to know about the physics
            registry_.get_or_emplace<Position>(entity).value.x = body->GetPosition().x;
            registry_.get_or_emplace<Position>(entity).value.y = body->GetPosition().y;
        }
    );

    registry.view<JointPtr, Drawable>().each(
        [](const auto entity, const JointPtr &joint, Drawable &drawable) {
            const auto pointA = joint->GetAnchorA();
            const auto pointB = joint->GetAnchorB();
            const auto distance = pointB - pointA;

            const auto angle = atan2f(distance.y, distance.x);
            const auto length = distance.Length();

            // TODO: Make rope thinner the closer it is to max length

            drawable.value->setPosition(
                pointA.x,
                pointA.y
            );

            drawable.value->setRotation(angle * 180.f / 3.145f);

            auto *rect = dynamic_cast<sf::RectangleShape *>(drawable.value.get());
            rect->setSize(sf::Vector2f(length, rect->getSize().y));
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

FixtureInfoPtr &Physics::makeFixture(
    entt::entity entity,
    sf::Shape *shape,
    entt::registry &reg,
    entt::entity bodyEntity
) {
    const BodyPtr *body = reg.try_get<BodyPtr>(bodyEntity);
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
    fixtureDef.friction = 0.0f;

    auto fix = std::make_shared<FixtureInfo>(
        FixtureInfo{
            FixturePtr((*body)->CreateFixture(&fixtureDef)),
            shape->getRotation(),
            shape->getPosition(),
            entity,
            bodyEntity
        }
    );

    fix->value->SetUserData(static_cast<void *>(fix.get()));
    return registry_.emplace<FixtureInfoPtr>(entity, fix);
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

void Physics::manageMovement(entt::entity entity, b2Body &body, Movement &movement) {
    if (!isOnFloor(entity)) {
        movement.direction = 0;
        return;
    }

    float vel = body.GetLinearVelocity().x;
    float desiredVel = 0;

    if (movement.direction == 0) {
        desiredVel = vel * movement.deceleration;
    }

    if (movement.direction > 0) {
        desiredVel = b2Min(vel + movement.acceleration, movement.vel());
    }

    if (movement.direction < 0) {
        desiredVel = b2Max(vel - movement.acceleration, movement.vel());
    }

    float velChange = desiredVel - body.GetLinearVelocity().x;
    float impulse = body.GetMass() * velChange;
    body.ApplyLinearImpulseToCenter(b2Vec2(impulse, 0), false);

    movement.direction = 0;
}

BodyPtr &Physics::makeBody(entt::entity entity, sf::Vector2f pos, float rot, b2BodyType type) {
    auto &body = registry_.emplace<BodyPtr>(entity, makeBody(pos, rot, type));
    return body;
}

b2World &Physics::getWorld() {
    return world_;
}

void Physics::rotateToPoint(b2Body &body, const sf::Vector2f &mousePos) {
    b2Vec2 toTarget = tob2(mousePos) - body.GetPosition();
    float desiredAngle = atan2f(-toTarget.x, toTarget.y);
    body.SetTransform(body.GetPosition(), desiredAngle);
}

void Physics::fireRope(Event<FireRope> event) {
    if (auto* rope = registry_.try_get<HoldingRope>(event.entity)) {
        registry_.destroy(rope->rope);
        registry_.remove<HoldingRope>(event.entity);
        return;
    }

    const auto &body = registry_.get<BodyPtr>(event.entity);
    auto startingPos = body->GetWorldPoint(tob2(event.eventDef.localPos));
    auto endingPos = tob2(event.eventDef.target);

    // TODO: Split this up into multiple smaller raycasts to avoid clipping
    auto scale = 10.f;

    // Increase the length the rope can be fired to
    auto dir = (endingPos - startingPos);
    dir = b2Vec2(dir.x / dir.Length(), dir.y / dir.Length());
    dir = b2Vec2(dir.x * scale, dir.y * scale);
    endingPos = startingPos + dir;

    // TODO Figure out why this doesn't segfault when c goes out of scope
    RopeHitCallback c(registry_, event.entity, world_, event.eventDef.localFireLoc);
    world_.RayCast(&c, startingPos, endingPos);
}

void Physics::jump(Event<Jump> event) {
    if (!isOnFloor(event.entity)) {
        return;
    }

    BodyPtr &body = registry_.get<BodyPtr>(event.entity);

    std::cout << "Gonna jump!" << std::endl;
    float impulse = body->GetMass() * event.eventDef.impulse;
    body->ApplyLinearImpulseToCenter(b2Vec2(0, impulse), false);
}

bool Physics::isOnFloor(entt::entity entity) {
    const auto *foot = registry_.try_get<FootSensor>(entity);

    if (!foot) {
        return true;
    }

    return foot->fixture->numberOfContacts > 0;
}

void Physics::teleport(Event<Teleport> event) {
    if (!registry_.has<BodyPtr>(event.entity)) {
        throw std::runtime_error("Must have a body to teleport");
    }

    BodyPtr &body = registry_.get<BodyPtr>(event.entity);

    // Translate anything attached to the body
    for (auto attachedEntity : registry_.get_or_emplace<Attachments>(event.entity).entities) {
        BodyPtr &attachedBody = registry_.get<BodyPtr>(attachedEntity);

        // Get the local pos of the attached entity so we can teleport it to the right location
        auto localPoint = body->GetLocalPoint(attachedBody->GetPosition());
        auto teleportLoc = tob2(event.eventDef.newLoc) + localPoint;
        attachedBody->SetAngularVelocity(0);
        attachedBody->SetLinearVelocity(b2Vec2_zero);
        attachedBody->SetTransform(teleportLoc, body->GetAngle());
    }

    // Translate the body itself
    body->SetAngularVelocity(0);
    body->SetLinearVelocity(b2Vec2_zero);
    body->SetTransform(tob2(event.eventDef.newLoc), body->GetAngle());
}


void BodyDeleter::operator()(b2Body *body) const {
    body->GetWorld()->DestroyBody(body);
}

void FixtureDeleter::operator()(b2Fixture *fixture) const {
    fixture->GetBody()->DestroyFixture(fixture);
}

void ContactListener::BeginContact(b2Contact *contact) {
    if (contact->GetFixtureA()->IsSensor() && contact->GetFixtureB()->IsSensor()) {
        return;
    }

    auto *fixA = static_cast<FixtureInfo *>(contact->GetFixtureA()->GetUserData());
    auto *fixB = static_cast<FixtureInfo *>(contact->GetFixtureB()->GetUserData());

    // TODO: Fix collisions with checkpoint and death zones
    fixA->numberOfContacts += 1;
    fixB->numberOfContacts += 1;

    if (auto checkpoint = registry_.try_get<Checkpoint>(fixA->bodyEntity)) {
        dispatcher_.enqueue(Event(fixB->bodyEntity, EnteredZone<Checkpoint> {*checkpoint}));
    }

    if (auto deathZone = registry_.try_get<DeathZone>(fixA->bodyEntity)) {
        dispatcher_.enqueue(Event(fixB->bodyEntity, EnteredZone<DeathZone> {*deathZone}));
    }


}

void ContactListener::EndContact(b2Contact *contact) {
    if (contact->GetFixtureA()->IsSensor() && contact->GetFixtureB()->IsSensor()) {
        return;
    }

    auto *fixA = static_cast<FixtureInfo *>(contact->GetFixtureA()->GetUserData());
    auto *fixB = static_cast<FixtureInfo *>(contact->GetFixtureB()->GetUserData());

    fixA->numberOfContacts -= 1;
    fixB->numberOfContacts -= 1;
}

ContactListener::ContactListener(entt::registry &registry, entt::dispatcher &dispatcher)
    : registry_(registry), dispatcher_(dispatcher) {}

void JointDeleter::operator()(b2Joint *joint) const {
    joint->GetBodyA()->GetWorld()->DestroyJoint(joint);
}

RopeHitCallback::RopeHitCallback(entt::registry &registry, entt::entity entity, b2World &world,
    sf::Vector2f localFireLoc) :
    registry_(registry), entity_(entity), world_(world), localFireLoc_(localFireLoc) {}

float RopeHitCallback::ReportFixture(
    b2Fixture *fixture,
    const b2Vec2 &point,
    const b2Vec2 &normal,
    float fraction) {

    entt::entity hitEntity = static_cast<FixtureInfo *>(fixture->GetUserData())->entity;

    // Not interested in attaching to sensors
    if (fixture->IsSensor()) {
        return -1;
    }

//    if (registry_.get<Attachments>(entity_).entities.contains(hitEntity)) {
//        return -1;
//    }

    b2RopeJointDef jointDef;
    jointDef.bodyA = fixture->GetBody();
    jointDef.localAnchorA = fixture->GetBody()->GetLocalPoint(point);

    jointDef.bodyB = registry_.get<BodyPtr>(entity_).get();
    // TODO Figure out how to get the arm position here from the event
    jointDef.localAnchorB = Physics::tob2(localFireLoc_);

    jointDef.maxLength = (point - jointDef.bodyB->GetWorldPoint(jointDef.localAnchorB)).Length();

    auto rope = registry_.create();

    auto joint = JointPtr(world_.CreateJoint(&jointDef));
    registry_.emplace<JointPtr>(rope, std::move(joint));

    auto width = 0.1f;

    auto drawable = Drawable{
        std::make_unique<sf::RectangleShape>(sf::Vector2f(16, width)),
        3
    };
    drawable.value->setOrigin(0, width / 2.f);

    registry_.emplace<Drawable>(rope, std::move(drawable));
    registry_.emplace<HoldingRope>(entity_, HoldingRope { sf::Vector2f(point.x, point.y), rope });

    return 0;
}
