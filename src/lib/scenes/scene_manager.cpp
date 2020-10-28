#include <spdlog/spdlog.h>
#include "scene_manager.h"
#include "main_menu_scene.h"
#include "level_scene.h"

SceneManager::SceneManager(sf::RenderWindow &window, std::optional<std::string> levelPath):
    window_(window)
{
    sceneDispatcher_.sink<ExitGame>().connect<&SceneManager::exitGame>(this);
    sceneDispatcher_.sink<StartLevel>().connect<&SceneManager::startLevel>(this);
    sceneDispatcher_.sink<FinishLevel>().connect<&SceneManager::finishLevel>(this);

    if (!levelPath) {
        scene_ = std::make_unique<MainMenuScene>("data/", window, sceneDispatcher_);
    } else {
        sceneDispatcher_.trigger<StartLevel>(levelPath.value());
    }
}

void SceneManager::run() {
    while (!shouldExit_) {
        scene_->step();
        window_.display();
        sceneDispatcher_.update();
    }
}

void SceneManager::exitGame(ExitGame event) {
    SPDLOG_INFO("Attempting to exit game");
    shouldExit_ = true;
}

void SceneManager::startLevel(const StartLevel &event) {
    scene_ = std::make_unique<LevelScene>(event.levelPath, window_, sceneDispatcher_);
}

void SceneManager::finishLevel(const FinishLevel &event) {
    SPDLOG_INFO("Finished level.");
    scene_ = std::make_unique<MainMenuScene>("data/", window_, sceneDispatcher_);
}
