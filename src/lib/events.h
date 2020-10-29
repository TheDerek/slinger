//
// Created by derek on 19/10/20.
//

#ifndef SLINGER_EVENTS_H
#define SLINGER_EVENTS_H

#include <entt/entity/fwd.hpp>
#include <SFML/Audio.hpp>

template <class T>
struct Event {
    entt::entity entity;
    T eventDef;

    inline Event(entt::entity entity, T eventDef): entity(entity), eventDef(eventDef) {};
};

template <class T>
struct EnteredZone {
    T zone;
};

struct Death {

};

struct FireRope {
    // Set by the creator
    //-------------------
    sf::Vector2f localPos;
    // The local location on the body to attach the rope to if it hits something
    sf::Vector2f localFireLoc;

    // Set by the input controller
    //----------------------------
    sf::Vector2f target;
};

struct Jump {
    float impulse = 80;
};

struct Teleport {
    sf::Vector2f newLoc;

    inline explicit Teleport(sf::Vector2f newLoc): newLoc(newLoc) {};
};

struct ExitGame {

};

struct StartLevel {
    std::string levelPath;

    inline explicit StartLevel(std::string level): levelPath(level) {};
};

struct FinishLevel {
    sf::Time completeTime;
};

struct ExitLevel {};

struct ResizeWindow {
    unsigned int width;
    unsigned int height;
};

inline std::string formatTime(const sf::Time& time) {
    int minutes = (int)time.asSeconds() / 60;
    float seconds = time.asSeconds() - (float) (minutes * 60);

    std::string display(7, '\0');
    std::snprintf(&display[0], display.size() + 1, "%02d:%04.1f", minutes, seconds);

    return display;
}

#endif //SLINGER_EVENTS_H
