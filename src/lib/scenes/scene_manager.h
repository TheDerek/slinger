#ifndef SLINGER_SCENE_MANAGER_H
#define SLINGER_SCENE_MANAGER_H


#include <memory>
#include <entt/signal/dispatcher.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <events.h>
#include "scene.h"

class SceneManager {
    std::unique_ptr<Scene> scene_;
    entt::dispatcher sceneDispatcher_;
    sf::RenderWindow& window_;
    bool shouldExit_ = false;

    void exitGame(ExitGame event);
    void startLevel(const StartLevel& event);
    void finishLevel(const FinishLevel& event);

public:
    SceneManager(sf::RenderWindow& window, std::optional<std::string> levelPath);
    void run();
};


#endif //SLINGER_SCENE_MANAGER_H
