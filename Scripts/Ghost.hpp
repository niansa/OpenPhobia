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

    bool hasValue() {
        return player;
    }
};

struct GhostAppearance {
    Color color;
};
struct GhostBehavior {
    Ghost *ghost;
    unsigned huntDuration = 30;
    unsigned sanityThreshold = 50;
    unsigned huntCooldown = 30;
    unsigned gracePeriod = 3;
    unsigned smudgeEffectDuration = 30; // <- TODO: verify
    float speedup = 0.0f;
    float agression = 0.0f;
    bool superHardThrows = false;

    virtual ~GhostBehavior() {}
    virtual float getCurrentSpeed() = 0;
    virtual PlayerWDistance getPlayerToChase() = 0;
    virtual unsigned getHuntMultiplier() = 0;
    virtual void onHuntStart() = 0;
};
namespace GhostBehaviors {
struct Default : public GhostBehavior {
    Default() {}
    float getCurrentSpeed() override;
    PlayerWDistance getPlayerToChase() override;
    unsigned getHuntMultiplier() override {return 0;}
    void onHuntStart() override;
};

struct Shade : public Default {
    Shade() {
        sanityThreshold = 35;
        agression = 0.25f;
    }
};
struct Spirit : public Default {
    Spirit() {
        smudgeEffectDuration = 120; // <- TODO: verify
    }
};
struct Poltergeist : public Default {
    Poltergeist() {
        superHardThrows = true;
    }
};
struct Oni : public Default {
    Oni() {
        superHardThrows = true;
        agression = 1.75f;
    }
};
struct Raiju : public Default {
    Raiju() {
        sanityThreshold = 65;
    }

    float getCurrentSpeed() override;
};
struct Daemon : public Default {
    Daemon() {
        sanityThreshold = 70;
        agression = 1.25f;
    }
};
struct Yokai : public Default {
    Yokai() {
        sanityThreshold = 80;
    }
};
struct Revenant : public Default {
    Revenant() {}
};
}

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
    const eastl::vector<PhysicsRaycastResult>& getCloseBodies() {
        return closeBodies;
    }

    void setState(GhostState nState);
    void throwBody(RigidBody *body);
    void useBody(RigidBody *body);
    float getAggression() const;
    bool walkTo(const Vector3& pos);
    bool chasePlayer();
    bool canSeePlayer(PlayerWDistance player);
    float getDistanceToPlayer(Player *player);
};
}
#endif // GHOST_HPP
