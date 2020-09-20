//
// Created by derek on 20/09/20.
//

#ifndef SLINGER_INPUT_MANAGER_H
#define SLINGER_INPUT_MANAGER_H

#include <SFML/Window.hpp>
#include <unordered_map>
#include <entt/entity/registry.hpp>

enum class InputAction {
    WALK_RIGHT,
    WALK_LEFT,
    JUMP
};

using InputComponent = std::unordered_map<sf::Keyboard::Key, InputAction>;

class InputManager {
public:
    InputManager(sf::Window& window);
    void handleInput(entt::registry&);
private:
    sf::Window& window_;

};


#endif //SLINGER_INPUT_MANAGER_H
