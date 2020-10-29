#include <spdlog/spdlog.h>
#include <fstream>

#include "scene_manager.h"
#include "main_menu_scene.h"
#include "level_scene.h"

const std::string SceneManager::LEVEL_PATH = "data/levels";
const std::string SceneManager::LEVEL_TIMES_PATH = "data/times.json";

SceneManager::SceneManager(sf::RenderWindow &window, std::optional<std::string> levelPath):
    window_(window)
{
    sceneDispatcher_.sink<ExitGame>().connect<&SceneManager::exitGame>(this);
    sceneDispatcher_.sink<StartLevel>().connect<&SceneManager::startLevel>(this);
    sceneDispatcher_.sink<FinishLevel>().connect<&SceneManager::finishLevel>(this);
    sceneDispatcher_.sink<ExitLevel>().connect<&SceneManager::exitLevel>(this);

    if (!levelPath) {
        openMainMenu();
    } else {
        sceneDispatcher_.trigger<StartLevel>(levelPath.value());
    }
}

void SceneManager::run() {
    while (!shouldExit_) {
        scene_->step();
        sceneDispatcher_.update();
    }
}

void SceneManager::exitGame(ExitGame event) {
    SPDLOG_INFO("Attempting to exit game");
    shouldExit_ = true;
}

void SceneManager::startLevel(const StartLevel &event) {
    lastLevelPath_ = event.levelPath;
    scene_ = std::make_unique<LevelScene>(event.levelPath, window_, sceneDispatcher_);
}

void SceneManager::finishLevel(const FinishLevel &event) {
    SPDLOG_INFO("Finished level {} with time {}", lastLevelPath_, formatTime(event.completeTime));
    writeLevelTime(lastLevelPath_, event.completeTime);
    openMainMenu();
}

void SceneManager::writeLevelTime(const std::string &levelPath, const sf::Time &levelTime) {
    auto times = getLevelTimes();

    if (times.contains(levelPath) && times[levelPath] < levelTime.asMilliseconds()) {
        SPDLOG_INFO("Not writing time of {} for {} because the existing time of {} is less",
            levelTime.asMilliseconds(), levelPath, times[levelPath].get<int>());
        return;
    }

    times[levelPath] = levelTime.asMilliseconds();

    std::ofstream outputFile(SceneManager::LEVEL_TIMES_PATH);
    outputFile << std::setw(4) << times << std::endl;

    SPDLOG_INFO("Written new time of {} for level {}", levelTime.asMilliseconds(), levelPath);
}

SceneManager::json SceneManager::getLevelTimes() {
    std::ifstream inputFile(SceneManager::LEVEL_TIMES_PATH);
    json times;
    inputFile >> times;
    inputFile.close();

    return times;
}

void SceneManager::openMainMenu() {
    scene_ = std::make_unique<MainMenuScene>(LEVEL_PATH, window_, sceneDispatcher_, getLevelTimes());
}

void SceneManager::exitLevel(const ExitLevel &event) {
    openMainMenu();
}
