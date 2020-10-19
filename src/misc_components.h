//
// Created by derek on 20/09/20.
//

#ifndef SLINGER_MISC_COMPONENTS_H
#define SLINGER_MISC_COMPONENTS_H

#include <set>
#include <SFML/System/Time.hpp>

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
    int x;
};

struct Checkpoint {
    sf::Vector2f respawnLoc;

    inline explicit Checkpoint(sf::Vector2f respawnLoc): respawnLoc(respawnLoc) {};
};

struct Respawnable {
    sf::Vector2f lastCheckpointLoc;
    sf::Time respawnTime;
    sf::Time currentRespawnTime;
    bool dead = false;

    inline explicit Respawnable(sf::Vector2f lastCheckpointLoc, sf::Time respawnTime):
        lastCheckpointLoc(lastCheckpointLoc), respawnTime(respawnTime) {};
};

struct Attachments {
    std::set<entt::entity> entities;
};

struct Position {
    sf::Vector2f value;
};



#endif //SLINGER_MISC_COMPONENTS_H
