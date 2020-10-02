//
// Created by derek on 20/09/20.
//

#include <iostream>
#include "input_manager.h"
#include "misc_components.h"

InputManager::InputManager(sf::RenderWindow &window, entt::dispatcher& dispatcher, entt::registry& registry) :
    window_(window), dispatcher_(dispatcher), registry_(registry) {

    registry_.on_construct<InputComponent>().connect<&InputManager::onAddInputComponent>(this);
}

UIAction InputManager::handleInput() {
    // Remove all key releases from the previous frame
    firstTimeKeyPresses_.clear();
    firstTimeButtonPresses_.clear();

    // Process events
    sf::Event event;
    while (window_.pollEvent(event)) {
        // Close window: exit
        if (event.type == sf::Event::Closed ||
            (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape)) {
            return UIAction::CLOSE_GAME;
        }

        if (event.type == sf::Event::KeyPressed) {
            firstTimeKeyPresses_.insert(event.key.code);
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            firstTimeButtonPresses_.insert(event.mouseButton.button);
        }
    }

    // Handle walking
    auto view = registry_.view<InputComponent>();
    for (auto entity : view) {
        for (const auto &kv : registry_.get<InputComponent>(entity)) {
            if(!std::visit(*this, kv.first)) {
                continue;
            }

            if(auto* action = std::get_if<InputAction>(&kv.second)) {
                if (Movement* movement = registry_.try_get<Movement>(entity)) {
                    handleMovement(entity, *action, *movement);
                }
            }

            // TODO: Find a way to automate this during compile time

            if(auto* jump = std::get_if<Jump>(&kv.second)) {
                dispatcher_.enqueue(Event(entity, *jump));
            }

            if(auto* fireRope = std::get_if<FireRope>(&kv.second)) {
                auto event = Event(entity, *fireRope);
                event.eventDef.target = window_.mapPixelToCoords(sf::Mouse::getPosition(window_));
                dispatcher_.enqueue(event);
            }
        }
    }

    return UIAction::NO_ACTION;
}

bool InputManager::operator() (sf::Keyboard::Key key) const {
    return sf::Keyboard::isKeyPressed(key);
}

bool InputManager::operator() (sf::Mouse::Button button) const {
    return sf::Mouse::isButtonPressed(button);
}

bool InputManager::operator() (JustPressed<sf::Keyboard::Key> key) const {
    return firstTimeKeyPresses_.contains(key.value);
}

bool InputManager::operator() (JustPressed<sf::Mouse::Button> button) const {
    return firstTimeButtonPresses_.contains(button.value);
}


void InputManager::handleMovement(entt::entity entity, InputAction action, Movement &movement) {
    if (action == InputAction::WALK_RIGHT) {
        movement.direction += 1;
    }

    if (action == InputAction::WALK_LEFT) {
        movement.direction -= 1;
    }
}

void InputManager::onAddInputComponent(entt::registry& registry, entt::entity entity) {
    std::cout << "wohoo adding input component" << std::endl;

    auto& inputComponent = registry.get<InputComponent>(entity);

}

void InputManager::operator()(InputAction action) const {

}
