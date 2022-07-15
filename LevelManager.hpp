#pragma once

namespace Scatter {
class LevelManager;
}
#ifndef LEVELMANAGER_HPP
#define LEVELMANAGER_HPP
#ifndef NDEBUG
#define URHO3D_DEBUG_ASSERT
#endif

#include "easyscript/Namespace.hpp"
#include "easyscript/SceneManager.hpp"
#include "GhostIdentity.hpp"

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Math/RandomEngine.h>
#include <Urho3D/RenderPipeline/RenderPipeline.h>
#include <Urho3D/RenderPipeline/BloomPass.h>
#include <Urho3D/RenderPipeline/SceneProcessor.h>



namespace Game {
class Ghost;
class Player;
class RoomBoundary;
class HouseBoundary;
struct GhostIdentity;

class LevelManager : public SceneManager {
    RandomEngine rng;
    eastl::string level = "OldBrickHouse";
    Ghost *ghost = nullptr;
    eastl::vector<Player*> players;
    eastl::vector<RoomBoundary*> rooms;
    HouseBoundary *house;
    GhostIdentity ghostIdentity;
    unsigned ghostIdentitySeed;

public:
    using SceneManager::SceneManager;

    void loadLevel(eastl::string nlevel) {
        level = nlevel;
        reloadLevel();
    }

    void Start();
    void reloadLevel();
    void loadEnv();
    unsigned getTeamSanity() const;
    bool isAnyPlayerInHouse();
    RoomBoundary *getPosRoom(Vector3 pos);
    RoomBoundary *getNodeRoom(Node *node);

    Ghost *getGhost() {
        URHO3D_ASSERT(!isGhostLoaded(), "Ghost requested before it loaded");
        return ghost;
    }
    bool isGhostLoaded() {
        return ghost;
    }
    const auto& getGhostIdentity() {
        return ghostIdentity;
    }
    const eastl::vector<Player*> &getPlayers() const {
        return players;
    }
    const eastl::vector<RoomBoundary*> &getRooms() const {
        return rooms;
    }
    auto &getRng() {
        return rng;
    }
    HouseBoundary *getHouse() const {
        return house;
    }
};
}
#endif // LEVELMANAGER_HPP
