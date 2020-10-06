//
// Created by derek on 19/09/20.
//

#include "illustrator.h"

#include <iostream>
#include <entt/entity/helper.hpp>

Illustrator::Illustrator(sf::RenderWindow &window, entt::registry &registry, entt::dispatcher &dispatcher) :
    window_(window),
    registry_(registry),
    dispatcher_(dispatcher),
    camera_(sf::Vector2f(0.f, 0.f), sf::Vector2f(800.f, -600.f) / 5.f)
{
    window_.setView(camera_);
    dispatcher_.sink<Event<FireRope>>().connect<&Illustrator::addRope>(*this);

    registry_.on_construct<Drawable>().connect<&Illustrator::onAddDrawable>(this);
    registry_.on_destroy<Drawable>().connect<&Illustrator::onAddDrawable>(this);


}

void Illustrator::onAddDrawable(entt::registry& registry, entt::entity entity) {
    // Sort drawable entities by z index
    registry_.sort<Drawable>(
        [](const auto &lhs, const auto &rhs) {
            return lhs.zIndex < rhs.zIndex;
        }
    );
}



void Illustrator::draw(entt::registry &registry) {
    window_.clear(sf::Color::Black);
    window_.setView(camera_);
    window_.setFramerateLimit(60);

    registry.view<Drawable>().each(
        [this, &registry](const auto entity, const Drawable &drawable) {
            auto &pos = drawable.value->getPosition();

            if (registry.has<entt::tag<"wrapView"_hs>>(entity)
                && absolute(camera_.getCenter() - pos) > absolute(camera_.getSize() / 2.f)) {
                drawable.value->setPosition(pos + 2.f * (camera_.getCenter() - pos));
            }

            window_.draw(*drawable.value);

            if (registry.try_get<Follow>(entity)) {
                camera_.setCenter(drawable.value->getPosition());
            }
        }
    );

    window_.display();
}

float Illustrator::round(float number, float multiple) {
    return ((number + multiple / 2) / multiple) * multiple;
}

Drawable Illustrator::makeRectangle(const sf::Vector2f &size, const sf::Vector2f &pos) const {
    Drawable d{std::make_unique<sf::RectangleShape>(sf::RectangleShape(size))};
    d.value->setPosition(pos);
    return d;
}

Drawable Illustrator::makeCircle(float radius, const sf::Vector2f &pos) const {
    Drawable d{std::make_unique<sf::CircleShape>(sf::CircleShape(radius))};
    d.value->setPosition(pos);
    return d;
}

sf::Vector2f Illustrator::absolute(const sf::Vector2f &vec) {
    return sf::Vector2f(abs(vec.x), abs(vec.y));
}

void Illustrator::addRope(const Event<FireRope> &event) {
    std::cout << "Drawing rope" << std::endl;
}

bool operator>(const sf::Vector2f &lhs, const sf::Vector2f &rhs) {
    return lhs.x > rhs.x || lhs.y > rhs.y;
}