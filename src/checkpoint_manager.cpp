//
// Created by derek on 19/10/20.
//

#include <iostream>
#include "checkpoint_manager.h"

CheckpointManager::CheckpointManager(entt::registry &registry, entt::dispatcher &dispatcher)
    : registry_(registry), dispatcher_(dispatcher)
{
    dispatcher_.sink<EnteredDeathZone>().connect<&CheckpointManager::onDeathZone>(this);
    dispatcher_.sink<EnteredCheckpoint>().connect<&CheckpointManager::onCheckpoint>(this);
}

void CheckpointManager::onDeathZone(const EnteredDeathZone &event) {
    auto respawnable = registry_.try_get<Respawnable>(event.entity);

    if (!respawnable || respawnable->dead) {
        return;
    }

    //respawnable->dead = true;
    std::cout << (long) event.entity << "checkpoint manager: entered the death zone!" << std::endl;
    dispatcher_.enqueue(Event<Teleport>(event.entity, Teleport(respawnable->lastCheckpointLoc)));
}

void CheckpointManager::onCheckpoint(const EnteredCheckpoint &event) {
    auto respawnable = registry_.try_get<Respawnable>(event.entity);

    if (!respawnable) {
        return;
    }

    std::cout << (long) event.entity << " new checkpoint!" << std::endl;
    respawnable->lastCheckpointLoc = event.eventDef.zone.respawnLoc;
}
