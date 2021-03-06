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
#include "../SphereCast.hpp"

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Math/RandomEngine.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Audio/SoundSource3D.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Navigation/DynamicNavigationMesh.h>



namespace Game {
class Player;
class EMFEmitter;
class Door;
class GhostIdentity;

struct GhostAppearance {
    Color color;
};

extern const eastl::vector<GhostAppearance> ghostAppearances;

class Ghost final : public LogicComponent {
    URHO3D_OBJECT(Ghost, LogicComponent);

    RandomEngine rng;
    Timer lowFreqStepTimer, stateTimer, lastHuntTimer, interactionTimer, roamTimer, footstepTimer;
    RigidBody* body;
    PhysicsWorld *physicsWorld;
    LevelManager *levelManager;
    const GhostAppearance *appearanceInfo;
    Node *appearance;
    AnimationController *animationController;
    SoundSource3D *feet;
    bool foot = 0;
    eastl::string state, nextState;
    eastl::optional<unsigned> nextStateIn;
    unsigned nextRoamIn = 0;
    bool nextRoamFar = false;
    GhostStateScript *stateScript = nullptr;
    eastl::unique_ptr<GhostBehavior> behavior = nullptr;
    DynamicNavigationMesh *navMesh;
    ea::vector<Vector3> currentPath;
    RoomBoundary *mostRecentRoom = nullptr;
    PlayerWDistance closestPlayer;
    Vector3 homePosition;
    eastl::vector<SphereCastResult> closeNodes;

    unsigned maxHuntSanity = 50;

    void updateClosestPlayer();
    void updateCloseBodies();
    void followPath();
    void playFootstep();

public:
    using LogicComponent::LogicComponent;

    void Start() override;
    void Update(float) override;
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
    float getStateDuration() {
        return nextStateIn.value_or(0);
    }
    PlayerWDistance getClosestPlayer() {
        return closestPlayer;
    }
    const eastl::vector<SphereCastResult>& getCloseNodes() {
        return closeNodes;
    }

    PlayerWDistance getPlayerWDistance(Player *p) {
        return {p, getDistanceToPlayer(p)};
    }
    void markNextRoamAsFar() {
        nextRoamFar = true;
    }
    const GhostIdentity& getIdentity() {
        return levelManager->getGhostIdentity();
    }

    void setState(const eastl::string& nState);
    InteractionType::Type tryInteract(InteractionType::Type type = InteractionType::any);
    void throwBody(RigidBody *body);
    void useNode(Node *node);
    void pushDoor(Door *door);
    float getAggression() const;
    void roam(bool respectTimer = true);
    bool walkTo(const Vector3& pos);
    bool canSeePlayer(PlayerWDistance player, bool includeElectronics = false);
    float getDistanceToPlayer(Player *player);
    EMFEmitter *spawnEMFSpot();
    RoomBoundary *getCurrentRoom();
    RoomBoundary *getFavoriteRoom();
};
}
#endif // GHOST_HPP
