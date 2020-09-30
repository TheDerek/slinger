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

template <class T>
struct JustPressed {
    T value;

    explicit JustPressed(T newValue) {
        value = newValue;
    }
};

namespace std {
    template <class T> struct hash<JustPressed<T>>
    {
        size_t operator()(const JustPressed<T>& x) const
        {
            return hash<T>()(x.value);
        }
    };
}

template <class T>
bool operator==(const JustPressed<T> &lhs, const JustPressed<T> &rhs) {
    return lhs.value == rhs.value;
}


using InputButton = std::variant<sf::Keyboard::Key, JustPressed<sf::Keyboard::Key>, JustPressed<sf::Mouse::Button>>;
using InputComponent = std::unordered_map<InputButton, InputAction>;

class InputManager {
public:
    InputManager(sf::Window& window);
    UIAction handleInput(entt::registry&, sf::Window&);
    void handleMovement(InputAction action, Movement& movement);

    bool operator() (sf::Keyboard::Key) const;
    bool operator() (sf::Mouse::Button) const;
    bool operator() (JustPressed<sf::Keyboard::Key>) const;
    bool operator() (JustPressed<sf::Mouse::Button>) const;

private:
    sf::Window& window_;
    std::set<sf::Keyboard::Key> firstTimeKeyPresses_;
    std::set<sf::Mouse::Button> firstTimeButtonPresses_;

};


#endif //SLINGER_INPUT_MANAGER_H
