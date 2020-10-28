#ifndef SLINGER_MAIN_MENU_SCENE_H
#define SLINGER_MAIN_MENU_SCENE_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <variant>
#include <SFML/Graphics/RectangleShape.hpp>
#include "scene.h"

class MenuItem {
    const static float PADDING;
    const static float MARGIN;

    sf::Text label_;
    sf::RectangleShape border_;
    const bool title_;

public:
    explicit MenuItem(sf::Text label_, bool title = false);

    float getWidth();
    float getHeight() const;
    void setPosition(float x, float y);
    void render(sf::RenderWindow& window);
};

struct MenuSpacer {
    float height = 35.f;
};

class Menu {
    const sf::Font& font_;
    sf::RenderWindow& window_;
    std::vector<std::variant<MenuItem, MenuSpacer>> items_;
    bool repositioned = false;
    float textHeight_;

public:
    explicit Menu(const sf::Font& font, sf::RenderWindow& window);
    void addItem(const std::string& label);
    void addTitle(const std::string& label);
    void addSpacer();
    void render();
    void reposition();

private:
    float getHeight();
};

class MainMenuScene : public Scene {
    const static float MARGIN;

    sf::RenderWindow& window_;
    const std::string& levelLocation_;
    sf::Font font_;
    sf::Text authorText_;
    sf::Text titleText_;
    Menu menu_;

public:
    explicit MainMenuScene(const std::string& levelLocation, sf::RenderWindow& window);
    void step() override;
};

#endif //SLINGER_MAIN_MENU_SCENE_H
