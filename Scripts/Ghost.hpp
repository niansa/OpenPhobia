#pragma once

namespace Game {
class Ghost;
}
#ifndef GHOST_HPP
#define GHOST_HPP
#include "../easyscript/Namespace.hpp"
#include "../LevelManager.hpp"

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

struct PlayerWDistance {
    Player *player = nullptr;
    float distance;
};

struct GhostAppearance {
    Color color;
};
struct GhostBehavior {
    Ghost *ghost;
    unsigned huntDuration = 30;
    unsigned sanityThreshold = 50;
    unsigned huntCooldown = 30;
    float speedup = 0.0f;

    virtual ~GhostBehavior() {}
    virtual float getCurrentSpeed() = 0;
    virtual PlayerWDistance getPlayerToChase() = 0;
};
namespace GhostBehaviors {
struct Default : public GhostBehavior {
    Default() {}
    virtual float getCurrentSpeed();
    virtual PlayerWDistance getPlayerToChase();
};
}

extern const eastl::vector<GhostAppearance> ghostAppearances;

class Ghost final : public LogicComponent {
    URHO3D_OBJECT(Ghost, LogicComponent);

    RandomEngine rng;
    Timer stepTimer, stateTimer, navigationTimer;
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

    float baseAgression = 1.f;
    unsigned maxHuntSanity = 50;

    void updateClosestPlayer();
    void followPath();

public:
    using LogicComponent::LogicComponent;

    virtual void Start() override;
    virtual void FixedUpdate(float) override;

    void setGhostBehavior(eastl::unique_ptr<GhostBehavior> nval) {
        behavior = std::move(nval);
    }
    void addHuntDuration(unsigned ival) {
        behavior->huntDuration += ival;
    }

    bool isVisible() {
        return appearance->IsEnabled();
    }

    bool hasNextState() {
        return nextStateIn != NAN;
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

    void setState(GhostState nState);
    void throwBody(RigidBody *body);
    void useBody(RigidBody *body);
    float getAggression() const;
    void walkTo(const Vector3& pos);
    void chasePlayer();
    bool canSeePlayer(PlayerWDistance player);
    float getDistanceToPlayer(Player *player);
};
}
#endif // GHOST_HPP
