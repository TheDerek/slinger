#ifndef SLINGER_LEVEL_SCENE_H
#define SLINGER_LEVEL_SCENE_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <physics.h>
#include <illustrator.h>
#include <input_manager.h>
#include <map_maker/map_maker.h>
#include <checkpoint_manager.h>
#include "scene.h"

class LevelScene : public Scene {
    sf::RenderWindow& window_;
    entt::dispatcher& sceneDispatcher_;

    entt::registry registry_;
    entt::dispatcher dispatcher_;
    sf::Clock deltaClock_;

    Physics physics_;
    Illustrator illustrator_;
    InputManager inputManager_;
    MapMaker mapMaker_;
    CheckpointManager checkpointManager_;

public:
    explicit LevelScene(const std::string& level, sf::RenderWindow& window, entt::dispatcher& sceneDispatcher);
    void step() override;
};


#endif //SLINGER_LEVEL_SCENE_H
