//
// Created by derek on 19/09/20.
//

#include "illustrator.h"

#include <iostream>
#include <random>
#include <entt/entity/helper.hpp>

void Illustrator::draw(entt::registry &registry)
{
    window_.clear(sf::Color::Black);
    window_.setView(camera_);
    window_.setFramerateLimit(60);

    registry.view<Drawable>().each(
            [this, &registry](const auto entity, const Drawable &drawable) {
                auto& pos = drawable.value->getPosition();

                if (registry.has<entt::tag<"wrapView"_hs>>(entity)
                    && absolute(camera_.getCenter() - pos) > absolute(camera_.getSize() / 2.f))
                {
                    drawable.value->setPosition(pos + 2.f * (camera_.getCenter() - pos));
                }

                window_.draw(*drawable.value);

                if (registry.try_get<Follow>(entity))
                {
                    camera_.setCenter(drawable.value->getPosition());
                }
            }
    );

    window_.display();
}

Illustrator::Illustrator(sf::RenderWindow &window) :
        window_(window),
        camera_(sf::Vector2f(0.f, 0.f), sf::Vector2f(800.f, -600.f) / 5.f)
{
    window_.setView(camera_);
}

float Illustrator::round(float number, float multiple)
{
    return ((number + multiple / 2) / multiple) * multiple;
}

Drawable Illustrator::makeRectangle(const sf::Vector2f &size, const sf::Vector2f &pos) const
{
    Drawable d{std::make_unique<sf::RectangleShape>(sf::RectangleShape(size))};
    d.value->setPosition(pos);
    return d;
}

Drawable Illustrator::makeCircle(float radius, const sf::Vector2f &pos) const
{
    Drawable d{std::make_unique<sf::CircleShape>(sf::CircleShape(radius))};
    d.value->setPosition(pos);
    return d;
}

std::vector<sf::Vector2f> Illustrator::getRandomViewPoints(const int count)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    auto points = std::vector<sf::Vector2f>(count);

    for (int i = 0; i < count; i++)
    {
        float x = (dist(mt) * camera_.getSize().x) + camera_.getCenter().x - camera_.getSize().x / 2.f;
        float y = (dist(mt) * camera_.getSize().y) + camera_.getCenter().y - camera_.getSize().y / 2.f;

        points.emplace_back(sf::Vector2f(x, y));
    }

    return points;
}

sf::Vector2f Illustrator::absolute(const sf::Vector2f& vec)
{
    return sf::Vector2f(abs(vec.x), abs(vec.y));
}

bool operator>(const sf::Vector2f &lhs, const sf::Vector2f &rhs)
{
    return lhs.x > rhs.x || lhs.y > rhs.y;
}