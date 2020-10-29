#ifndef SLINGER_SCENE_MANAGER_H
#define SLINGER_SCENE_MANAGER_H


#include <memory>
#include <entt/signal/dispatcher.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <events.h>
#include <nlohmann/json.hpp>
#include "scene.h"

class SceneManager {
    using json = nlohmann::json;

    const static std::string LEVEL_PATH;
    const static std::string LEVEL_TIMES_PATH;

    std::unique_ptr<Scene> scene_;
    entt::dispatcher sceneDispatcher_;
    sf::RenderWindow& window_;
    std::string lastLevelPath_;
    bool shouldExit_ = false;

    static json getLevelTimes();
    static void writeLevelTime(const std::string& levelPath, const sf::Time& levelTime);

    void openMainMenu();

    // Event handlers
    void exitGame(ExitGame event);
    void startLevel(const StartLevel& event);
    void finishLevel(const FinishLevel& event);

public:
    SceneManager(sf::RenderWindow& window, std::optional<std::string> levelPath);
    void run();
};


#endif //SLINGER_SCENE_MANAGER_H
