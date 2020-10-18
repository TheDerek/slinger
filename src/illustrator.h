//
// Created by derek on 19/09/20.
//

#ifndef SLINGER_ILLUSTRATOR_H
#define SLINGER_ILLUSTRATOR_H

#include <SFML/Graphics.hpp>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include "misc_components.h"

//TODO: Replace with container struct that has the
// shape and the Z index which we will use to sort
// the entities via https://bit.ly/3cA73dw
using ShapePtr = std::unique_ptr<sf::Shape>;

struct Drawable {
    std::unique_ptr<sf::Shape> value;
    int zIndex = 0;
};

class Illustrator
{
    sf::View camera_;
    sf::RenderWindow& window_;
    entt::dispatcher& dispatcher_;
    entt::registry& registry_;
    sf::Font font_;
    sf::Text text_;

public:
    explicit Illustrator(sf::RenderWindow &window, entt::registry &registry, entt::dispatcher &dispatcher);

    void draw(entt::registry &registry);

    Drawable makeRectangle(const sf::Vector2f& size, const sf::Vector2f& pos) const;
    Drawable makeCircle(float radius, const sf::Vector2f& pos) const;


private:
    float round(float number, float multiple);
    sf::Vector2f absolute(const sf::Vector2f& vec);
    void addRope(const Event<FireRope>& event);
    void onPlayerDeath(const Event<Death>& event);

    void onAddDrawable(entt::registry &registry, entt::entity entity);
};

inline bool operator> (const sf::Vector2f& lhs, const sf::Vector2f& rhs);


#endif //SLINGER_ILLUSTRATOR_H
