//
// Created by derek on 20/09/20.
//

#ifndef SLINGER_MISC_COMPONENTS_H
#define SLINGER_MISC_COMPONENTS_H

#include <set>
#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>

struct Movement {
    float speed = 10;
    float acceleration = 3;
    float deceleration = 0.8f;
    float direction = 0;

    [[nodiscard]] float vel() const {
        return direction * speed;
    }
};

struct Follow
{
    int x = 1;
};

struct HoldingRope {
    sf::Vector2f ropeLoc;
    entt::entity rope;
};

struct DeathZone {
    bool spikes = false;
};

struct Checkpoint {
    sf::Vector2f respawnLoc;
    bool finish = false;

    inline explicit Checkpoint(sf::Vector2f respawnLoc, bool finish): respawnLoc(respawnLoc), finish(finish) {};
};

struct Respawnable {
    sf::Vector2f lastCheckpointLoc;
    sf::Time respawnTime;
    sf::Time currentRespawnTime;
    bool dead = false;
    bool finished = false;

    inline explicit Respawnable(sf::Vector2f lastCheckpointLoc, sf::Time respawnTime):
        lastCheckpointLoc(lastCheckpointLoc), respawnTime(respawnTime) {};
};

class Timeable {
    sf::Clock clock_;
    bool clockStarted_ = false;
    std::string display_;

public:
    inline explicit Timeable(): display_(7, '\0') {};

    [[nodiscard]] inline bool hasStarted() const {
        return clockStarted_;
    }
    void startIfNotStarted() {
        if (clockStarted_) {
            return;
        }

        clockStarted_ = true;
        clock_.restart();
    }

    [[nodiscard]] const std::string& formatTime() {
        if (hasStarted()) {
            int minutes = clock_.getElapsedTime().asSeconds() / 60;
            float seconds = clock_.getElapsedTime().asSeconds() - (float)(minutes * 60);

            std::snprintf(&display_[0], display_.size() + 1, "%02d:%04.1f", minutes, seconds);
        } else {
            display_ = "00:00.0";
        }
        return display_;
    }
};

struct Attachments {
    std::set<entt::entity> entities;
};

struct Position {
    sf::Vector2f value;
};



#endif //SLINGER_MISC_COMPONENTS_H
