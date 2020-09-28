//
// Created by derek on 20/09/20.
//

#ifndef SLINGER_INPUT_MANAGER_H
#define SLINGER_INPUT_MANAGER_H

#include <SFML/Window.hpp>
#include <unordered_map>
#include <entt/entity/registry.hpp>
#include <set>
#include <variant>

enum class InputAction {
    WALK_RIGHT,
    WALK_LEFT,
    JUMP,
    FIRE_ROPE
};

enum class UIAction {
    CLOSE_GAME,
    NO_ACTION
};

using InputComponent = std::unordered_map<std::variant<sf::Keyboard::Key, sf::Mouse::Button>, InputAction>;

class InputManager {
public:
    InputManager(sf::Window& window);
    UIAction handleInput(entt::registry&, sf::Window&);
private:
    sf::Window& window_;
    std::set<sf::Keyboard::Key> firstTimeKeyPresses_;

};


#endif //SLINGER_INPUT_MANAGER_H
