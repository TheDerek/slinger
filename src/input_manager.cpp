//
// Created by derek on 20/09/20.
//

#include <iostream>
#include "input_manager.h"
#include "misc_components.h"

InputManager::InputManager(sf::Window &window) : window_(window) {

}

UIAction InputManager::handleInput(entt::registry &registry, sf::Window &window) {
    // Remove all key releases from the previous frame
    firstTimeKeyPresses_.clear();

    // Process events
    sf::Event event;
    while (window.pollEvent(event)) {
        // Close window: exit
        if (event.type == sf::Event::Closed ||
            (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape)) {
            return UIAction::CLOSE_GAME;
        }

        // Close window: exit
        if (event.type == sf::Event::KeyPressed) {
            firstTimeKeyPresses_.insert(event.key.code);
        }
    }

    // Handle input for individual entities
    auto view = registry.view<InputComponent>();
    for (auto entity : view) {
        Movement *walkDir = registry.try_get<Movement>(entity);

        if (!walkDir) {
            continue;
        }

        walkDir->direction = 0;
        walkDir->jumping = false;

        for (const auto &kv : registry.get<InputComponent>(entity)) {
            if (!(sf::Keyboard::isKeyPressed(kv.first) || firstTimeKeyPresses_.contains(kv.first))) {
                continue;
            }

            if (kv.second == InputAction::WALK_RIGHT) {
                walkDir->direction += 1;
            }

            if (kv.second == InputAction::WALK_LEFT) {
                walkDir->direction -= 1;
            }

            if (kv.second == InputAction::JUMP && firstTimeKeyPresses_.contains(kv.first)) {
                walkDir->jumping = true;
            }
        }
    }

    return UIAction::NO_ACTION;
}
