#include <SFML/Window/Event.hpp>
#include <utility>
#include <spdlog/spdlog.h>
#include <events.h>
#include <filesystem>
#include "main_menu_scene.h"

const float MainMenuScene::MARGIN = 10;
const float MenuItem::MARGIN = 10;
const float MenuItem::PADDING = 5;

MainMenuScene::MainMenuScene(
    const std::string& levelLocation,
    sf::RenderWindow &window,
    entt::dispatcher& sceneDispatcher,
    const nlohmann::json& times
):
    levelLocation_(levelLocation),
    sceneDispatcher_(sceneDispatcher),
    window_(window),
    menu_(font_, window)
{
    if (!font_.loadFromFile("data/LiberationMono-Regular.ttf"))
    {
        throw std::runtime_error("Could not locate font");
    }

    titleText_.setFont(font_);
    titleText_.setCharacterSize(50);
    titleText_.setFillColor(sf::Color::White);
    titleText_.setString("Slinger!");

    menu_.addTitle("Select level to play");
    for (const auto& level : getLevels(levelLocation, times)) {
        menu_.addItem(level.getDisplayName(), StartLevel(level.getPath()));
    }
    menu_.addSpacer();
    menu_.addItem("How to Play", std::monostate{});
    menu_.addItem("Exit", ExitGame());

    authorText_.setFont(font_);
    authorText_.setCharacterSize(16);
    authorText_.setFillColor(sf::Color::White);
    authorText_.setString("Game made by Derek, please contact mail@derek.tech for feedback!");

    reposition(window_.getSize().x, window_.getSize().y);
}

void MainMenuScene::step() {
    sf::Event event;
    while (window_.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
        {
           SPDLOG_INFO("Closing game from main menu window exit button");
           sceneDispatcher_.enqueue(ExitGame {});
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            menu_.handleMousePress(event.mouseButton.x, event.mouseButton.y, sceneDispatcher_);
        }

        // catch the resize events
        if (event.type == sf::Event::Resized)
        {
           reposition(event.size.width, event.size.height);
        }

        if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape) {
            SPDLOG_INFO("Closing game from main menu window with escape key");
            sceneDispatcher_.enqueue(ExitGame());
        }
    }

    window_.clear(sf::Color(100, 100, 100));
    window_.draw(titleText_);
    menu_.render();
    window_.draw(authorText_);
}

void MainMenuScene::reposition(int width, int height) {
    // update the view to the new size of the window
    sf::FloatRect visibleArea(0, 0, width, height);
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

std::vector<LevelInfo> MainMenuScene::getLevels(const std::string& levelsLoc, const nlohmann::json& times) {
    std::vector<LevelInfo> levels;

    for(auto& p: std::filesystem::directory_iterator(levelsLoc)) {
        if (p.path().extension() == ".svg") {
            std::optional<sf::Time> time;
            if (times.contains(p.path())) {
                time = sf::milliseconds(times[p.path()]);
            }

            levels.emplace_back(LevelInfo(p.path(), time));
        }
    }

    return levels;
}

Menu::Menu(const sf::Font& font, sf::RenderWindow& window):
    font_(font), window_(window)
{

}

void Menu::addItem(const std::string& label, MenuAction action) {
    sf::Text text;
    text.setFont(font_);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Black);
    text.setString(label);

    items_.emplace_back(MenuItem(text, action));
}

void Menu::addTitle(const std::string &label) {
    sf::Text text;
    text.setFont(font_);
    text.setCharacterSize(27);
    text.setFillColor(sf::Color::White);
    text.setString(label);

    items_.emplace_back(MenuItem(text, std::monostate{}, true));
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

    window_.display();
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

void Menu::handleMousePress(int x, int y, entt::dispatcher &dispatcher) {
    for (auto& item : items_) {
        if (auto *menuItem = std::get_if<MenuItem>(&item)) {
            menuItem->handleMousePress((float) x, (float) y, dispatcher);
        }
    }
}

MenuItem::MenuItem(sf::Text label, MenuAction action, bool title):
    label_(std::move(label)),
    title_(title),
    menuAction_(action)
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

bool MenuItem::isClicked(float x, float y) {
    if (title_) {
        return false;
    }

    return border_.getGlobalBounds().contains(x, y);
}

void MenuItem::handleMousePress(float x, float y, entt::dispatcher &dispatcher) {
    if (!isClicked(x, y)) {
        return;
    }

    SPDLOG_INFO("'{}' button clicked", std::string(label_.getString()));

    if (std::holds_alternative<ExitGame>(menuAction_)) {
        dispatcher.enqueue(std::get<ExitGame>(menuAction_));
    }

    if (std::holds_alternative<StartLevel>(menuAction_)) {
        dispatcher.enqueue(std::get<StartLevel>(menuAction_));
    }
}

LevelInfo::LevelInfo(std::filesystem::path path, std::optional<sf::Time> completionTime):
    path_(path), completionTime_(completionTime)
{
    displayName_ = path.replace_extension("").filename().generic_string() + " ";

    if (completionTime) {
        displayName_ += "[" + formatTime(completionTime.value()) + "]";
    } else {
        displayName_ += "[Not attempted]";
    }
}

const std::string &LevelInfo::getDisplayName() const {
    return displayName_;
}

const std::filesystem::path &LevelInfo::getPath() const {
    return path_;
}
