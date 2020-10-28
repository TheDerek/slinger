#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include <iostream>
#include <memory>

#include <spdlog/spdlog.h>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>
#include <level_scene.h>
#include <scenes/main_menu_scene.h>

bool shouldExit = false;
std::unique_ptr<Scene> scene;

void exitGame() {
    shouldExit = true;
}

void startLevel(const StartLevel& event, sf::RenderWindow& window) {
    scene = std::make_unique<LevelScene>(event.levelPath, window);
}

/**
 * Fetch the map from the program arguments, return an empty optional if no map path is
 * provided
 * @param argc
 * @param argv
 * @return the path to the map supplied in the given arguments, or empty if none
 */
std::optional<std::string> getMap(int argc, char *argv[]) {
    if (argc < 2) {
        return std::optional<std::string>();
    }

    if (argc > 3) {
        throw std::runtime_error("Incorrect number of arguments supplied");
    }

    return std::string(argv[1]);
}

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("%Y-%m-%d %@ %! [%l] %v");

    auto mapPath = getMap(argc, argv);
    SPDLOG_INFO("Starting game, using map: {}", mapPath.value_or("No map found"));

    auto settings = sf::ContextSettings();
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(
        sf::VideoMode(1000, 800),
        "Slinger DEV build"
    );
    window.setKeyRepeatEnabled(false);

    entt::dispatcher sceneDispatcher;
    sceneDispatcher.sink<ExitGame>().connect<&exitGame>();
    sceneDispatcher.sink<StartLevel>().connect<&startLevel>(window);


    if (!mapPath) {
        scene = std::make_unique<MainMenuScene>("data/", window, sceneDispatcher);
    } else {
        scene = std::make_unique<LevelScene>(mapPath.value(), window);
    }

    // Start the game loop
    while (!shouldExit) {
        scene->step();
        window.display();

        sceneDispatcher.update();
    }
}

