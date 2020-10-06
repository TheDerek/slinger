//
// Created by derek on 20/09/20.
//

#ifndef SLINGER_MISC_COMPONENTS_H
#define SLINGER_MISC_COMPONENTS_H


struct Movement {
    float speed = 60;
    float acceleration = 20;
    float deceleration = 0.90f;
    float jumpVel = 80;

    bool jumping = false;
    float direction = 0;

    [[nodiscard]] float vel() const {
        return direction * speed;
    }
};

struct HoldingRope {
    sf::Vector2f ropeLoc;
    entt::entity rope;
};


template <class T>
struct Event {
    entt::entity entity;
    T eventDef;

    Event(entt::entity entity, T eventDef): entity(entity), eventDef(eventDef) {};
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

#endif //SLINGER_MISC_COMPONENTS_H
