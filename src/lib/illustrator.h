//
// Created by derek on 19/09/20.
//

#ifndef SLINGER_ILLUSTRATOR_H
#define SLINGER_ILLUSTRATOR_H

#include <SFML/Graphics.hpp>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include "misc_components.h"
#include "events.h"

struct Drawable {
    std::unique_ptr<sf::Shape> value;
    int zIndex = 0;
};

class Illustrator
{
    sf::View camera_;
    sf::View uiView_;
    sf::RenderWindow& window_;
    entt::dispatcher& dispatcher_;
    entt::registry& registry_;
    sf::Font font_;
    sf::Text text_;

public:
    explicit Illustrator(sf::RenderWindow &window, entt::registry &registry, entt::dispatcher &dispatcher);
    void draw(entt::registry &registry);

private:
    sf::Vector2f absolute(const sf::Vector2f& vec);
    void addRope(const Event<FireRope>& event);
    void onPlayerDeath(const Event<Death>& event);
    void onAddDrawable(entt::registry &registry, entt::entity entity);
    void resizeWindow(ResizeWindow event);
};

inline bool operator> (const sf::Vector2f& lhs, const sf::Vector2f& rhs);


#endif //SLINGER_ILLUSTRATOR_H
