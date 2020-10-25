//
// Created by derek on 19/10/20.
//

#include <iostream>
#include <spdlog/spdlog.h>
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

    despawn(event.entity, *respawnable);
}

void CheckpointManager::onCheckpoint(const EnteredCheckpoint &event) {
    auto respawnable = registry_.try_get<Respawnable>(event.entity);

    if (!respawnable) {
        return;
    }

    if (Timeable* timeable = registry_.try_get<Timeable>(event.entity)) {
        timeable->startIfNotStarted();
    }

    respawnable->lastCheckpointLoc = event.eventDef.zone.respawnLoc;
    SPDLOG_INFO("Entity {} reached new checkpoint at ({}, {})", event.entity, respawnable->lastCheckpointLoc.x, respawnable->lastCheckpointLoc.y);

}

void CheckpointManager::update(sf::Time delta) {
    registry_.view<Respawnable>().each(
        [this, delta](const auto entity, Respawnable &respawnable) {
            if (!respawnable.dead) {
                return;
            }

            if (respawnable.currentRespawnTime > sf::Time::Zero) {
                respawnable.currentRespawnTime -= delta;
            } else {
                respawn(entity, respawnable);
            }
        }
    );
}

void CheckpointManager::respawn(entt::entity entity, Respawnable &respawnable) {
    respawnable.dead = false;
    registry_.emplace_or_replace<Follow>(entity);
    dispatcher_.enqueue(Event<Teleport>(entity, Teleport(respawnable.lastCheckpointLoc)));
    SPDLOG_INFO("Entity {} has has respawned at ({}, {})", entity, respawnable.lastCheckpointLoc.x, respawnable.lastCheckpointLoc.y);
}

void CheckpointManager::despawn(entt::entity entity, Respawnable& respawnable) {
    registry_.remove_if_exists<Follow>(entity);

    // Remove any ropes the entity is holding on to
    for (auto attachedEntity : registry_.get_or_emplace<Attachments>(entity).entities) {
        if (auto *rope = registry_.try_get<HoldingRope>(attachedEntity)) {
            registry_.destroy(rope->rope);
            registry_.remove<HoldingRope>(attachedEntity);
        }
    }

    respawnable.dead = true;
    respawnable.currentRespawnTime = respawnable.respawnTime;
    SPDLOG_INFO("Entity {} has died, will respawn in {} seconds", entity, respawnable.currentRespawnTime.asSeconds());
}
