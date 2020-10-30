#ifndef SLINGER_TUTORIAL_SCENE_H
#define SLINGER_TUTORIAL_SCENE_H


#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <entt/signal/dispatcher.hpp>
#include "scene.h"

class TutorialScene : public Scene {
    const static std::string HOW_TO_PLAY_IMAGE_PATH;

    sf::RenderWindow& window_;
    entt::dispatcher& sceneDispatcher_;

    sf::Texture image_;
    sf::Sprite tutorial_;

public:
    TutorialScene(sf::RenderWindow& window, entt::dispatcher& sceneDispatcher);
    void step() override;
private:
    void reposition(unsigned int width, unsigned int height);
};


#endif //SLINGER_TUTORIAL_SCENE_H
