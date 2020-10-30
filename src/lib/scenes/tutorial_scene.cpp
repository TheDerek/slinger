#include <SFML/Window/Event.hpp>
#include <spdlog/spdlog.h>
#include <events.h>
#include "tutorial_scene.h"

const std::string TutorialScene::HOW_TO_PLAY_IMAGE_PATH = "data/how-to-play.png";

TutorialScene::TutorialScene(sf::RenderWindow &window, entt::dispatcher& sceneDispatcher):
    window_(window),
    sceneDispatcher_(sceneDispatcher)
{
    if (!image_.loadFromFile(HOW_TO_PLAY_IMAGE_PATH))
    {
        throw std::runtime_error("Could not find image: " + HOW_TO_PLAY_IMAGE_PATH);
    }


    tutorial_.setTexture(image_);
    tutorial_.setOrigin(image_.getSize().x/2.f, image_.getSize().y/2.f);

    reposition(window_.getSize().x, window_.getSize().y);
}

void TutorialScene::step() {
    sf::Event event;
    while (window_.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
        {
            SPDLOG_INFO("Closing game from tutorial");
            sceneDispatcher_.enqueue(ExitGame {});
        }

        // catch the resize events
        if (event.type == sf::Event::Resized)
        {
            reposition(event.size.width, event.size.height);
        }

        if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape) {
            SPDLOG_INFO("Returning to menu from tutorial");
            sceneDispatcher_.enqueue(ExitLevel());
        }
    }


    window_.clear();
    window_.draw(tutorial_);
    window_.display();
}

void TutorialScene::reposition(unsigned int width, unsigned int height) {
    sf::FloatRect visibleArea(0, 0, width, height);
    window_.setView(sf::View(visibleArea));

    float scaleX = (float)window_.getSize().x/(float)image_.getSize().x;
    float scaleY = scaleX;

    if (scaleY * (float)image_.getSize().y > (float)window_.getSize().y) {
        scaleY = (float)window_.getSize().y/(float)image_.getSize().y;
        scaleX = scaleY;
    }

    tutorial_.setPosition(
        window_.getSize().x / 2.f,
        window_.getSize().y / 2.f
    );
    tutorial_.setScale(scaleX, scaleY);
}
