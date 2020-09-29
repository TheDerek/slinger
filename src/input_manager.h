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
#include "misc_components.h"

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


struct JustPressedKey {
    sf::Keyboard::Key value;

    friend bool operator== (const JustPressedKey& lhs, const JustPressedKey& rhs);
};


namespace std {
    template <> struct hash<JustPressedKey>
    {
        size_t operator()(const JustPressedKey& x) const
        {
            return hash<int>()(x.value);
        }
    };
}


using InputButton = std::variant<JustPressedKey, sf::Keyboard::Key, sf::Mouse::Button>;
using InputComponent = std::unordered_map<InputButton, InputAction>;

class InputManager {
public:
    InputManager(sf::Window& window);
    UIAction handleInput(entt::registry&, sf::Window&);
    void handleMovement(InputAction action, Movement& movement);

    bool operator() (sf::Keyboard::Key) const;
    bool operator() (sf::Mouse::Button) const;
    bool operator() (JustPressedKey) const;
private:
    sf::Window& window_;
    std::set<sf::Keyboard::Key> firstTimeKeyPresses_;
    std::set<sf::Mouse::Button> firstTimeButtonPresses_;

};


#endif //SLINGER_INPUT_MANAGER_H
