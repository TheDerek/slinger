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

        if (event.type == sf::Event::KeyPressed) {
            firstTimeKeyPresses_.insert(event.key.code);
        }
    }

    // Handle walking
    registry.view<InputComponent, Movement>().each(
        [&registry, this](const auto entity, const InputComponent& input, Movement& movement) {
        for (const auto &kv : registry.get<InputComponent>(entity)) {

            if (!(sf::Keyboard::isKeyPressed(kv.first) || firstTimeKeyPresses_.contains(kv.first))) {
                continue;
            }

            if (kv.second == InputAction::WALK_RIGHT) {
                movement.direction += 1;
            }

            if (kv.second == InputAction::WALK_LEFT) {
                movement.direction -= 1;
            }

            if (kv.second == InputAction::JUMP && firstTimeKeyPresses_.contains(kv.first)) {
                movement.jumping = true;
            }
        }
    });

    return UIAction::NO_ACTION;
}