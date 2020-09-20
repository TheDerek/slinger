//
// Created by derek on 20/09/20.
//

#include <iostream>
#include "input_manager.h"
#include "misc_components.h"

InputManager::InputManager(sf::Window &window): window_(window) {

}

void InputManager::handleInput(entt::registry& registry) {
    auto view = registry.view<InputComponent>();

    for (auto entity : view) {
        WalkDir* walkDir = registry.try_get<WalkDir>(entity);

        if (walkDir) {
            walkDir->value = 0;
        }

        for (const auto& kv : registry.get<InputComponent>(entity)) {

            if (!sf::Keyboard::isKeyPressed(kv.first)) {
                continue;
            }

            if (kv.second == InputAction::WALK_RIGHT && walkDir) {
                walkDir->value += 1;
            }

            if (kv.second == InputAction::WALK_LEFT && walkDir) {
                walkDir->value -= 1;
            }
        }
    }
}
