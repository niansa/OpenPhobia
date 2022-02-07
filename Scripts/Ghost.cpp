#include "Ghost.hpp"
#include "Player.hpp"
#include "Useable.hpp"
#include "EMFEmitter.hpp"
#include "Lightswitch.hpp"
#include "GhostBehavior.hpp"
#include "RoomBoundary.hpp"
#include "GhostReveal.hpp"
#include "../SphereCastMultiple.hpp"
#include "../LevelManager.hpp"
#include "../GhostState.hpp"

#include <Urho3D/Math/Ray.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Math/RandomEngine.h>
#include <Urho3D/Math/Ray.h>
#include <Urho3D/Physics/KinematicCharacterController.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Navigation/DynamicNavigationMesh.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource3D.h>
#include <Urho3D/Math/Color.h>
#include <Urho3D/Resource/ResourceCache.h>
#ifndef NDEBUG
#   include <Urho3D/Input/Input.h>
#endif



namespace Game {
const eastl::vector<GhostAppearance> ghostAppearances =
{
    {Color(0.25f, 0.25f, 0.25f)},
    {Color(0.358871f, 0.0f, 0.0999211f)}
};

void Ghost::Start() {
    appearance = GetNode()->GetChild("Appearance");
    levelManager = GetGlobalVar("LevelManager").GetCustom<LevelManager*>();
    animationController = appearance->GetChild("Model")->GetComponent<AnimationController>();
    navMesh = GetScene()->GetChild("SceneMain")->GetComponent<DynamicNavigationMesh>();
    // Create kinematic controller
    kinematicController = GetNode()->CreateComponent<KinematicCharacterController>();
    kinematicController->SetCollisionLayerAndMask(10, 1);
    kinematicController->SetStepHeight(0.05);
    // Find physics world
    physicsWorld = GetScene()->GetComponent<PhysicsWorld>();
    // Get random ghost appearance
    appearanceInfo = &(ghostAppearances[rng.GetUInt(0, ghostAppearances.size())]);
    // Set ghost appearance
    eastl::vector<AnimatedModel*> models;
    appearance->GetChild("Model")->GetComponents<AnimatedModel>(models);
    for (auto model : models) {
        Material *mat = model->GetMaterial();
        mat->SetShaderParameter("MatEmissiveColor", Variant(appearanceInfo->color));
    }
    // Set initial home position
    homePosition = GetNode()->GetWorldPosition();
    getCurrentRoom();
    // Set initial ghost state
    setState("Local");
    // Start its animation
    animationController->Play("Objects/Ghost/Animations/Idle.ani", 1, true);
}

void Ghost::FixedUpdate(float) {
    // Keep following the currentPath
    followPath();
    // Interact
    if ((isVisible() || rng.GetBool(0.25f * getAggression())) && interactionTimer.GetMSec(false) > behavior->interactionCooldown) {
        // Throw or use an object in range
        if (!closeBodies.empty()) {
            for (unsigned triesLeft = 25; triesLeft != 0; triesLeft--) {
                auto result = closeBodies[rng.GetInt(0, closeBodies.size()-1)];
                if (result.distance_ > 1.0f) {
                    continue;
                }
                auto body = result.body_;
                if (body) {
                    if (body->GetNode()->HasTag("GhostInteractable") && body->GetNode()->GetParent()->GetName() != "Hand") {
                        if (body->GetNode()->HasTag("Grabbable")) {
                            throwBody(body);
                        }
                        if (body->GetNode()->HasTag("Useable")) {
                            useBody(body);
                        }
                        interactionTimer.Reset();
                        break;
                    }
                }
            }
        }
    }
#   ifndef NDEBUG
    // Debug keys
    auto input = GetSubsystem<Input>();
    if (input->GetKeyPress(Key::KEY_M)) {
        setState("Roam");
    } else if (input->GetKeyPress(Key::KEY_R)) {
        setState("Reveal");
    } else if (input->GetKeyPress(Key::KEY_L)) {
        setState("Local");
    } else if (input->GetKeyPress(Key::KEY_H)) {
        setState("Hunt");
    }
#   endif
    // Low frequency timed clode
    if (lowFreqStepTimer.GetMSec(false) > 2500) {
        // Update list of close objects
        updateCloseBodies();
        // Reset step timer
        lowFreqStepTimer.Reset();
    }
    // Potential state switch
    trySwitchState();
}

void Ghost::setState(const eastl::string& nState) {
    // Backup old state
    auto oState = eastl::move(state);
    // Set new state
    state = nState;
    // Stop walking
    currentPath.clear();
    // Get invisible
    appearance->SetDeepEnabled(false);

    // Check that behavior has loaded
    if (behavior) {
        // Remove old state script
        if (stateScript) {
            stateScript->Deinitialize();
            GetNode()->RemoveComponent("Ghost"+oState);
        }
        // Create new one
        stateScript = reinterpret_cast<GhostStateScript*>(GetNode()->CreateComponent("Ghost"+nState));
        stateScript->ghost = this;
        stateScript->Initialize();
    } else {
        // Behavior is not loaded - just start roaming after 15 seconds
        setNextState("Roam", 15000);
    }
}

void Ghost::throwBody(RigidBody *body) {
    body->SetKinematic(false);
    // Get random direction
    auto dir = rng.GetDirectionVector3()*1.5f;
    if (dir.y_ < 0) {
        dir.y_ = -dir.y_;
    }
    // Get random power
    auto power = behavior->superHardThrows?rng.GetFloat(0.75f, 2.0f):rng.GetFloat(0.25f, 1.75f);
    body->SetLinearVelocity(dir*power);
    // Get emf level to emit
    EMFLevel level;
    if (behavior->hasEvidence(Evidence::EMFLevelFive) && rng.GetBool(0.25f)) {
        level = EMFLevel::five;
    } else {
        level = EMFLevel::throw_;
    }
    // Make it emit emf
    auto emitter = body->GetNode()->GetOrCreateComponent<EMFEmitter>();
    emitter->setLevel(level);
    emitter->timeoutIn(20);
}

void Ghost::useBody(RigidBody *body) {
    auto node = body->GetNode();
    auto script = static_cast<Useable *>(node->GetComponent(node->GetName()));
    // Check if node is lightswitch
    if (node->GetName() == "Lightswitch") {
        if (script->isTurnedOn()) {
            // Might as well just dim it
            if (rng.GetBool(0.25f)) {
                static_cast<Lightswitch*>(script)->ghostyDim(true);
            } else {
                script->GhostUse();
            }
        } else {
            // Reduce chance to turn it on
            if (rng.GetBool(0.5f)) {
                script->GhostUse();
            }
        }
    } else {
        // Just use it whatever it is
        script->GhostUse();
    }
    // Get emf level to emit
    EMFLevel level;
    if (behavior->hasEvidence(Evidence::EMFLevelFive) && rng.GetBool(0.25f)) {
        level = EMFLevel::five;
    } else {
        level = EMFLevel::touch;
    }
    // Make it emit emf
    auto emitter = body->GetNode()->GetOrCreateComponent<EMFEmitter>();
    emitter->setLevel(level);
    emitter->timeoutIn(20);
}

float Ghost::getAggression() const {
    return Max(0.01f, (-(float(levelManager->getTeamSanity())-100))/100) * baseAgression * behavior->agression;
}

void Ghost::roam() {
    if (currentPath.empty()) {
        // Get random location around the ghost
        Vector3 nPos;
        unsigned tries = 0;
        do {
            constexpr float maxDist = 10.0f,
                            minDist = 5.0f;
            nPos = rng.GetVector3(Vector3(minDist, 0.0f, minDist), Vector3(maxDist, 0.0f, maxDist));
            if (rng.GetBool(0.5f)) {
                nPos.x_ = -nPos.x_;
            }
            if (rng.GetBool(0.5f)) {
                nPos.z_ = -nPos.z_;
            }
            // Make sure to not get stuck
            if (tries++ == 10000) {
                break;
            }
            // Go there
        } while (!walkTo(GetNode()->GetWorldPosition() + nPos));
    }
}

void Ghost::updateClosestPlayer() {
    closestPlayer.player = nullptr;
    closestPlayer.distance = 100.f;
    bool canSeeClosestPlayer = false;
    for (auto player : levelManager->getPlayers()) {
        if (!player->isInsideHouse()) {
            continue;
        }
        auto p = getPlayerWDistance(player);
        auto canSeeP = canSeePlayer(p);
        if (p.distance < closestPlayer.distance || (!canSeeClosestPlayer && canSeeP)) {
            closestPlayer = p;
            canSeeClosestPlayer = canSeeP;
        }
    }
}

void Ghost::updateCloseBodies() {
    constexpr float range = 25.0f;
    SphereCastMultipleDS(physicsWorld, closeBodies, Ray(GetNode()->GetWorldPosition(), GetNode()->GetWorldDirection()), range, range);
    // Hotfix: Put in distance values manually
    for (auto& result : closeBodies) {
        result.distance_ = (GetNode()->GetWorldPosition() - result.position_).Length();
    }
}

bool Ghost::walkTo(const Vector3& pos) {
    navMesh->FindPath(currentPath, GetNode()->GetWorldPosition(), pos);
    return !currentPath.empty();
}

void Ghost::followPath() {
    if (!currentPath.empty()) {
        auto nextWaypoint = currentPath[0];
        nextWaypoint.y_ = GetNode()->GetWorldPosition().y_;

#       ifndef NDEBUG
        // Show debug waypoint thing (slow)
        for (auto node : GetScene()->GetChildren(false)) {
            if (node->GetName() == "DebugNavBox") {
                node->Remove();
            }
        }
        auto* cache = GetSubsystem<ResourceCache>();
        auto test = GetScene()->CreateChild();
        test->SetName("DebugNavBox");
        test->SetPosition(nextWaypoint);
        test->SetScale({0.25f, 0.25f, 0.25f});
        auto mesh = test->CreateComponent<StaticModel>();
        mesh->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
#       endif

        // Get distance and stuff
        float move = behavior->getCurrentSpeed() / 60.f;
        float distance = (GetNode()->GetWorldPosition() - nextWaypoint).Length();
        if (move > distance)
            move = distance;

        // Remove waypoint if it was reached
        if (distance < 0.1f) {
            navigationTimer.Reset();
            currentPath.pop_front();
            return;
        } else if (navigationTimer.GetMSec(false) > 5000.f) {
            kinematicController->Warp(nextWaypoint);
            navigationTimer.Reset();
            currentPath.pop_front();
            return;
        }

        // Rotate toward next waypoint to reach and move
        GetNode()->LookAt(nextWaypoint, Vector3::UP);
        kinematicController->SetWalkDirection(GetNode()->GetWorldDirection() * move);
    } else {
        kinematicController->SetWalkDirection(Vector3::ZERO);
    }
}

bool Ghost::canSeePlayer(PlayerWDistance player) {
    // Check that player is valid
    if (!player.player) {
        return false;
    }
    // Check that player is inside house
    if (!player.player->isInsideHouse()) {
        return false;
    }
    // Check if player is in line of sight
    auto vectorToPlayer = player.player->getHead()->GetWorldPosition() - GetNode()->GetWorldPosition();
    auto vectorForward = GetNode()->GetWorldDirection();
    ea::vector<PhysicsRaycastResult> hits;
    physicsWorld->Raycast(hits, Ray(GetNode()->GetWorldPosition(), vectorToPlayer.Normalized()), vectorToPlayer.Length());
    if (hits.size() <= 1) {
        return true;
    }
    // Check if player has electronics turned on
    if (player.distance < behavior->electricVisionRange) {
        auto playerHand = player.player->getHand();
        if (playerHand && playerHand->HasTag("Useable")) {
            auto script = static_cast<Useable *>(playerHand->GetComponent(playerHand->GetName()));
            if (script->isTurnedOn()) {
                return true;
            }
        }
    }
    // Finally, give up looking for the player :P
    return false;
}

float Ghost::getDistanceToPlayer(Player *player) {
    return (GetNode()->GetWorldPosition() - player->GetNode()->GetWorldPosition()).Length();
}

RoomBoundary *Ghost::getCurrentRoom() {
    auto fres = levelManager->getNodeRoom(GetNode());
    if (fres) {
        mostRecentRoom = fres;
    } else {
        fres = mostRecentRoom;
    }
    return fres;
}

RoomBoundary *Ghost::getFavoriteRoom() {
    return levelManager->getPosRoom(homePosition);;
}
}
