#pragma once

namespace Game {
class Ghost;
}
#ifndef GHOST_HPP
#define GHOST_HPP
#include "../easyscript/Namespace.hpp"
#include "../LevelManager.hpp"
#include "../GhostBehavior.hpp"

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Math/RandomEngine.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/KinematicCharacterController.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Navigation/DynamicNavigationMesh.h>



namespace Game {
class Player;

enum class GhostState {
    local,
    roaming,
    reveal,
    hunt
};

struct GhostAppearance {
    Color color;
};

extern const eastl::vector<GhostAppearance> ghostAppearances;

class Ghost final : public LogicComponent {
    URHO3D_OBJECT(Ghost, LogicComponent);

    RandomEngine rng;
    Timer stepTimer, lowFreqStepTimer, stateTimer, navigationTimer;
    KinematicCharacterController* kinematicController;
    PhysicsWorld *physicsWorld;
    LevelManager *levelManager;
    const GhostAppearance *appearanceInfo;
    Node *appearance;
    AnimationController *animationController;
    GhostState state;
    GhostState nextState;
    float nextStateIn = NAN;
    Timer lastHuntTimer;
    eastl::unique_ptr<GhostBehavior> behavior = nullptr;
    DynamicNavigationMesh *navMesh;
    ea::vector<Vector3> currentPath;
    PlayerWDistance closestPlayer;
    Vector3 homePosition;
    eastl::vector<PhysicsRaycastResult> closeBodies;

    float baseAgression = 1.f;
    unsigned maxHuntSanity = 50;

    void updateClosestPlayer();
    void updateCloseBodies();
    void followPath();

public:
    using LogicComponent::LogicComponent;

    void Start() override;
    void FixedUpdate(float) override;

    GhostBehavior *getGhostBehavior() {
        return behavior.get();
    }
    void setGhostBehavior(eastl::unique_ptr<GhostBehavior> nval) {
        behavior = std::move(nval);
    }

    bool isVisible() {
        return appearance->IsEnabled();
    }

    bool hasNextState() {
        return !isnan(nextStateIn);
    }
    void clearNextState() {
        nextStateIn = NAN;
    }
    void switchState() {
        clearNextState();
        setState(nextState);
    }
    void trySwitchState() {
        if (hasNextState() && stateTimer.GetMSec(false) > nextStateIn) {
            switchState();
        }
    }
    void setNextState(GhostState nState, float in) {
        stateTimer.Reset();
        nextState = nState;
        nextStateIn = in;
    }
    GhostState getState() {
        return state;
    }
    PlayerWDistance getClosestPlayer() {
        return closestPlayer;
    }
    const eastl::vector<PhysicsRaycastResult>& getCloseBodies() {
        return closeBodies;
    }

    PlayerWDistance getPlayerWDistance(Player *p) {
        return {p, getDistanceToPlayer(p)};
    }

    void setState(GhostState nState);
    void throwBody(RigidBody *body);
    void useBody(RigidBody *body);
    float getAggression() const;
    bool walkTo(const Vector3& pos);
    bool chasePlayer();
    bool canSeePlayer(PlayerWDistance player);
    float getDistanceToPlayer(Player *player);
    RoomBoundary *getCurrentRoom();
};
}
#endif // GHOST_HPP
