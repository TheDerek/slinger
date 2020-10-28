#include <SFML/Window/Event.hpp>
#include <utility>
#include "main_menu_scene.h"

const float MainMenuScene::MARGIN = 10;
const float MenuItem::MARGIN = 10;
const float MenuItem::PADDING = 5;

MainMenuScene::MainMenuScene(const std::string& levelLocation, sf::RenderWindow &window):
    levelLocation_(levelLocation),
    window_(window),
    menu_(font_, window)
{
    window_.setFramerateLimit(60);

    if (!font_.loadFromFile("data/LiberationMono-Regular.ttf"))
    {
        throw std::runtime_error("Could not locate font");
    }

    titleText_.setFont(font_);
    titleText_.setCharacterSize(50);
    titleText_.setFillColor(sf::Color::White);
    titleText_.setString("Slinger!");

    menu_.addTitle("Select level to play");
    menu_.addItem("Beginning [Not Completed]");
    menu_.addItem("Fall [Not Completed]");
    menu_.addSpacer();
    menu_.addItem("How to Play");
    menu_.addItem("Exit");

    authorText_.setFont(font_);
    authorText_.setCharacterSize(16);
    authorText_.setFillColor(sf::Color::White);
    authorText_.setString("Game made by Derek, please contact mail@derek.tech for feedback!");
}

void MainMenuScene::step() {
    sf::Event event;
    while (window_.pollEvent(event)) {
        // catch the resize events
        if (event.type == sf::Event::Resized)
        {
            // update the view to the new size of the window
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            window_.setView(sf::View(visibleArea));

            // Update the title position
            auto x = (window_.getSize().x / 2.f) - titleText_.getLocalBounds().width / 2.f;
            auto y = MainMenuScene::MARGIN;
            titleText_.setPosition(x, y);

            // Update the menu position
            menu_.reposition();

            // Update the author position
            x = (window_.getSize().x / 2.f) - authorText_.getLocalBounds().width / 2.f;
            y = window_.getSize().y - authorText_.getLocalBounds().height - MARGIN;
            authorText_.setPosition(x, y);
        }
    }

    window_.clear(sf::Color(100, 100, 100));
    window_.draw(titleText_);
    menu_.render();
    window_.draw(authorText_);
}

Menu::Menu(const sf::Font& font, sf::RenderWindow& window):
    font_(font), window_(window)
{

}

void Menu::addItem(const std::string& label) {
    sf::Text text;
    text.setFont(font_);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Black);
    text.setString(label);

    items_.emplace_back(MenuItem(text));
}

void Menu::addTitle(const std::string &label) {
    sf::Text text;
    text.setFont(font_);
    text.setCharacterSize(27);
    text.setFillColor(sf::Color::White);
    text.setString(label);

    items_.emplace_back(MenuItem(text, true));
}

void Menu::render() {
    if (!repositioned) {
        repositioned = true;
        reposition();
    }

    for (auto& item : items_) {
        if (!std::holds_alternative<MenuItem>(item)) {
            continue;
        }

        std::get<MenuItem>(item).render(window_);
    }
}

void Menu::reposition() {
    const float height = getHeight();

    float y = (window_.getSize().y / 2.f) - height /2.f;

    for (size_t i = 0; i < items_.size(); i++) {
        auto& item = items_.at(i);

        if (auto* menuItem = std::get_if<MenuItem>(&item)) {
            float x = (window_.getSize().x / 2.f) - (menuItem->getWidth() /2.f);
            menuItem->setPosition(x, y);
            y += menuItem->getHeight();
        }

        if (auto* spacer = std::get_if<MenuSpacer>(&item)) {
            y += spacer->height;
        }
    }
}

float Menu::getHeight() {
    if (items_.empty()) {
        return 0;
    }

    float height = 0;
    for (const auto& item : items_) {
        if (auto* menuItem = std::get_if<MenuItem>(&item)) {
            height += menuItem->getHeight();
        }

        if (auto* spacer = std::get_if<MenuSpacer>(&item)) {
            height += spacer->height;
        }
    }

    return height;
}

void Menu::addSpacer() {
    items_.emplace_back(MenuSpacer {});
}

MenuItem::MenuItem(sf::Text label, bool title):
    label_(std::move(label)),
    title_(title)
{
    if (title_) {
        border_.setFillColor(sf::Color::White);
    } else {
        border_.setFillColor(sf::Color::Transparent);
        border_.setOutlineThickness(3);
        border_.setOutlineColor(sf::Color::Black);
    }
}

void MenuItem::render(sf::RenderWindow &window) {
    window.draw(border_);
    window.draw(label_);
}

float MenuItem::getWidth() {
    return label_.getLocalBounds().width;
}

void MenuItem::setPosition(float x, float y) {
    label_.setPosition(x, y);

    float width = label_.getLocalBounds().width + PADDING * 2.f;

    if (title_) {
        border_.setPosition(x - PADDING, y + label_.getLocalBounds().height + PADDING);
        border_.setSize(sf::Vector2f(
            width,
            1.f
        ));
    } else {
        border_.setPosition(x - PADDING/2.f, y);
        border_.setSize(sf::Vector2f(
            width,
            label_.getLocalBounds().height + PADDING * 2.f
        ));
    }
}

float MenuItem::getHeight() const {
    return label_.getLocalBounds().height + PADDING * 2 + MARGIN;
}
