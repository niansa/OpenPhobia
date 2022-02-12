#pragma once

namespace Game {
class Ghost;
}
#ifndef GHOST_HPP
#define GHOST_HPP
#include "../easyscript/Namespace.hpp"
#include "../LevelManager.hpp"
#include "../GhostBehavior.hpp"
#include "../GhostState.hpp"

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

struct GhostAppearance {
    Color color;
};

extern const eastl::vector<GhostAppearance> ghostAppearances;

class Ghost final : public LogicComponent {
    URHO3D_OBJECT(Ghost, LogicComponent);

    RandomEngine rng;
    Timer lowFreqStepTimer, stateTimer, navigationTimer, lastHuntTimer, interactionTimer;
    KinematicCharacterController* kinematicController;
    PhysicsWorld *physicsWorld;
    LevelManager *levelManager;
    const GhostAppearance *appearanceInfo;
    Node *appearance;
    AnimationController *animationController;
    eastl::string state, nextState;
    eastl::optional<unsigned> nextStateIn;
    GhostStateScript *stateScript = nullptr;
    eastl::unique_ptr<GhostBehavior> behavior = nullptr;
    DynamicNavigationMesh *navMesh;
    ea::vector<Vector3> currentPath;
    RoomBoundary *mostRecentRoom = nullptr;
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
        return state == "Reveal" || state == "Hunt"; //TODO: Don't just depent on these
    }

    bool hasNextState() {
        return nextStateIn.has_value();
    }
    void clearNextState() {
        nextStateIn.reset();
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
    void setNextState(const eastl::string& nState, unsigned in) {
        stateTimer.Reset();
        nextState = nState;
        nextStateIn = in;
    }
    const eastl::string& getState() {
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

    void setState(const eastl::string& nState);
    InteractionType::Type tryInteract(InteractionType::Type type = InteractionType::any);
    void throwBody(RigidBody *body);
    void useBody(RigidBody *body);
    float getAggression() const;
    void roam();
    bool walkTo(const Vector3& pos);
    bool canSeePlayer(PlayerWDistance player, bool includeElectronics = false);
    float getDistanceToPlayer(Player *player);
    RoomBoundary *getCurrentRoom();
    RoomBoundary *getFavoriteRoom();
};
}
#endif // GHOST_HPP
