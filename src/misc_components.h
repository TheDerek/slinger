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

#endif //SLINGER_MISC_COMPONENTS_H
