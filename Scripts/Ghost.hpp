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



namespace Game {
enum class GhostState {
    local,
    roaming,
    reveal,
    hunt
};

struct GhostAppearance {
    Color color;
};
struct GhostBehavior {
    unsigned huntDuration;
    unsigned sanityThreshold = 50;
    unsigned huntCooldown = 30;
};

extern const eastl::vector<GhostAppearance> ghostAppearances;

class Ghost final : public LogicComponent {
    URHO3D_OBJECT(Ghost, LogicComponent);

    RandomEngine rng;
    Timer stepTimer, stateTimer;
    KinematicCharacterController* kinematicController;
    PhysicsWorld *physicsWorld;
    LevelManager *levelManager;
    const GhostAppearance *appearanceInfo;
    Node *appearance;
    AnimationController *animationController;
    GhostState nextState;
    float nextStateIn = NAN;
    Timer lastHuntTimer;
    GhostBehavior behavior;

    float baseAgression = 1.f;
    unsigned maxHuntSanity = 50;

public:
    GhostState state;

    using LogicComponent::LogicComponent;

    virtual void Start() override;
    virtual void FixedUpdate(float) override;

    void setGhostBehavior(const GhostBehavior& nval) {
        behavior = nval;
    }
    void addHuntDuration(unsigned ival) {
        behavior.huntDuration += ival;
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

    void setState(GhostState nState);
    void throwBody(RigidBody *body);
    void useBody(RigidBody *body);
    float getAggression() const;
};
}
#endif // GHOST_HPP
