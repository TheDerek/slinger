//
// Created by derek on 19/10/20.
//

#ifndef SLINGER_CHECKPOINT_MANAGER_H
#define SLINGER_CHECKPOINT_MANAGER_H


#include <entt/entity/registry.hpp>
#include <entt/entt.hpp>
#include "events.h"
#include "misc_components.h"

namespace {
    using EnteredDeathZone = Event<EnteredZone<DeathZone>>;
    using EnteredCheckpoint = Event<EnteredZone<Checkpoint>>;
}

class CheckpointManager {
    entt::registry& registry_;
    entt::dispatcher& dispatcher_;

    void onDeathZone(const EnteredDeathZone &event);
    void onCheckpoint(const EnteredCheckpoint &event);
    void respawn(entt::entity, Respawnable& respawnable);
    void despawn(entt::entity, Respawnable& respawnable);
public:
    CheckpointManager(entt::registry &registry, entt::dispatcher &dispatcher);
    void update(sf::Time delta);
};


#endif //SLINGER_CHECKPOINT_MANAGER_H
