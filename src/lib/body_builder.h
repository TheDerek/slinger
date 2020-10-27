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
#include "animation.h"

class ShapeBuilder;

struct ShapePrototype {
    std::unique_ptr<sf::Shape> shape;
    bool draw = false;
    bool makeFixture = false;
    bool sensor = false;
    bool footSensor = false;
    float density = 1;
    float friction = 0.2f;
    int zIndex = 0;
    std::optional<Animation> animation;
};

struct AttachPrototype {
    entt::entity body;
    float localAnchorAX;
    float localAnchorAY;
    float localAnchorBX;
    float localAnchorBY;
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
    std::optional<AttachPrototype> attachPrototype_;

public:
    explicit BodyBuilder(entt::registry &registry, Physics &physics);
    BodyBuilder& addShape(ShapePrototype);
    ShapeBuilder addRect(float width, float height);
    ShapeBuilder addPolygon(const std::vector<sf::Vector2f>& points);
    BodyBuilder &setPos(float x, float y);
    BodyBuilder &setType(b2BodyType type);
    BodyBuilder &setRot(float rot);
    BodyBuilder &setFixedRotation(bool fixedRotation);
    BodyBuilder &attach(entt::entity body, float localAnchorAX, float localAnchorAY, float localAnchorBX, float localAnchorBY);
    entt::entity create();
};

class ShapeBuilder {
private:
    BodyBuilder* bodyBuilder_;
    ShapePrototype prototype_;

public:

    static ShapeBuilder CreateRect(float width, float height);
    static ShapeBuilder CreatePolygon(const std::vector<sf::Vector2f>& points);
    void setBodyBuilder(BodyBuilder *bodyBuilder);


    ShapeBuilder& setPos(float x, float y);
    ShapeBuilder& setRot(float x);
    ShapeBuilder& draw();
    ShapeBuilder& makeFixture();
    ShapeBuilder& setDensity(float density);
    ShapeBuilder& setFriction(float friction);
    ShapeBuilder& setSensor();
    ShapeBuilder& setColor(sf::Color color);
    ShapeBuilder& setFootSensor();
    ShapeBuilder& setZIndex(int z);
    ShapeBuilder& setAnimation(Animation animation);

    /**
     * Attach the built shape to the body. This is undefined if this builder was not created with a
     * BodyBuilder pointer.
     * @return
     */
    BodyBuilder& attachToBody();

    /**
     * Create the shape without any physics attributes
     * @param registry
     * @param entity
     * @return the entity the created shape is attached to
     */
    entt::entity create(entt::registry& registry, std::optional<entt::entity> entity = std::optional<entt::entity>());

private:
    ShapeBuilder(std::unique_ptr<sf::Shape> shape);
};


#endif //SLINGER_BODY_BUILDER_H
