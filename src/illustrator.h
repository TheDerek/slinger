//
// Created by derek on 19/09/20.
//

#ifndef SLINGER_ILLUSTRATOR_H
#define SLINGER_ILLUSTRATOR_H

#include <SFML/Graphics.hpp>
#include <entt/entity/registry.hpp>

//TODO: Replace with container struct that has the
// shape and the Z index which we will use to sort
// the entities via https://bit.ly/3cA73dw
using Drawable = std::unique_ptr<sf::Shape>;

struct Follow
{
    int x = 1;
};

class Illustrator
{
    sf::View camera_;
    sf::RenderWindow& window_;

public:
    explicit Illustrator(sf::RenderWindow& window);

    void draw(entt::registry &registry);

    Drawable makeRectangle(const sf::Vector2f& size, const sf::Vector2f& pos) const;
    Drawable makeCircle(float radius, const sf::Vector2f& pos) const;

    std::vector<sf::Vector2f> getRandomViewPoints(int count);

private:
    float round(float number, float multiple);
    sf::Vector2f absolute(const sf::Vector2f& vec);
};

inline bool operator> (const sf::Vector2f& lhs, const sf::Vector2f& rhs);


#endif //SLINGER_ILLUSTRATOR_H
