#include "level_scene.h"

LevelScene::LevelScene(const std::string &level, sf::RenderWindow &window):
    window_(window),
    physics_(registry_, dispatcher_),
    illustrator_(window_, registry_, dispatcher_),
    inputManager_(window_, dispatcher_, registry_),
    mapMaker_(registry_, physics_),
    checkpointManager_(registry_, dispatcher_)
{
    mapMaker_.make(level);
}

void LevelScene::step() {
    inputManager_.handleInput();

    // Clear screen
    window_.clear(sf::Color::White);

    // Get the mouse pos
    sf::Vector2f mousePos = window_.mapPixelToCoords(sf::Mouse::getPosition(window_));

    auto delta = deltaClock_.restart();
    checkpointManager_.update(delta);
    physics_.handlePhysics(registry_, delta.asSeconds(), mousePos);
    illustrator_.draw(registry_);
}