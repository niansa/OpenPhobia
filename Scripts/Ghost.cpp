#include "Ghost.hpp"
#include "Player.hpp"
#include "Useable.hpp"
#include "EMFEmitter.hpp"
#include "Lightswitch.hpp"
#include "Door.hpp"
#include "SFX.hpp"
#include "GhostBehavior.hpp"
#include "RoomBoundary.hpp"
#include "HouseBoundary.hpp"
#include "GhostReveal.hpp"
#include "../SphereCastMultiple.hpp"
#include "../LevelManager.hpp"
#include "../GhostState.hpp"

#include <random>
#include <algorithm>
#include <Urho3D/Math/Ray.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Math/RandomEngine.h>
#include <Urho3D/Math/Ray.h>
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
    body = GetNode()->CreateComponent<RigidBody>();
    body->SetCollisionLayerAndMask(0, 0);
    body->SetUseGravity(false);
    body->SetTrigger(true);
    body->SetMass(1.0f);
    // Find physics world
    physicsWorld = GetScene()->GetComponent<PhysicsWorld>();
    // Get random ghost appearance
    appearanceInfo = &(ghostAppearances[rng.GetUInt(0, ghostAppearances.size())]);
    // Set ghost appearance
    eastl::vector<AnimatedModel*> models;
    appearance->GetChild("Model")->GetComponents<AnimatedModel>(models);
    for (auto model : models) {
        Material *mat = model->GetMaterial();
        mat->SetShaderParameter("MatEmissiveColor", Variant(appearanceInfo->color)); // DNV
    }
    // Create feet
    feet = GetNode()->CreateChild("Feet")->CreateComponent<SoundSource3D>();
    // Set initial home position
    homePosition = GetNode()->GetWorldPosition();
    getCurrentRoom();
    // Set initial ghost state
    setState("Local");
    // Start its animation
    animationController->Play("Objects/Ghost/Animations/Idle.ani", 1, true);
}

void Ghost::Update(float) {
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
    } else if (input->GetKeyPress(Key::KEY_I)) {
        tryInteract();
    }
#   endif
}

void Ghost::FixedUpdate(float) {
    // Keep following the currentPath
    followPath();
    // Low frequency timed clode
    auto lfst = lowFreqStepTimer.GetMSec(false);
    if (lfst > 2500) {
        // Interact
        if ((isVisible() || rng.GetBool(0.01f * getAggression())) && interactionTimer.GetMSec(false) > behavior->interactionCooldown) {
            tryInteract();
        }
        // Run behavior update
        behavior->FrequentUpdate(lfst);
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

InteractionType::Type Ghost::tryInteract(InteractionType::Type type) {
    // Shuffle close bodies  Note: EA STL shuffle seems to be broken so STL one is in use instead
    std::shuffle(closeBodies.begin(), closeBodies.end(), std::mt19937(rng.GetUInt()));
    // Try to throw/touch an object in range
    for (auto result : closeBodies) {
        if (result.distance_ > 2.5f) {
            continue;
        }
        auto body = result.body_;
        if (body) {
            for (auto node = body->GetNode(); node; node = node->GetParent()) {
                if (node->HasTag("GhostInteractable") && node->GetParent()->GetName() != "Hand") {
                    if (type & InteractionType::touch) {
                        if (rng.GetBool(0.5f) && node->HasComponent<Door>()) {
                            behavior->onInteraction(InteractionType::touch);
                            pushDoor(node->GetComponent<Door>());
                            interactionTimer.Reset();
                            return InteractionType::touch;
                        }
                        if (rng.GetBool(0.5f) && node->HasTag("Useable")) {
                            behavior->onInteraction(InteractionType::touch);
                            useNode(node);
                            interactionTimer.Reset();
                            return InteractionType::touch;
                        }
                    }
                    if (type & InteractionType::throw_ && node->HasTag("Grabbable")) {
                        behavior->onInteraction(InteractionType::throw_);
                        throwBody(body);
                        interactionTimer.Reset();
                        return InteractionType::throw_;
                    }
                }
            }
        }
    }
    // Give up
    return InteractionType::none;
}

void Ghost::throwBody(RigidBody *body) {
    body->SetKinematic(false);
    // Get random direction
    auto dir = rng.GetDirectionVector3()*1.5f;
    if (dir.y_ < 0) {
        dir.y_ = -dir.y_;
    }
    // Get random power
    auto power = behavior->getThrowPower();
    body->SetLinearVelocity(dir*power);
    // Make it emit emf
    auto emitter = body->GetNode()->GetOrCreateComponent<EMFEmitter>();
    emitter->setLevel(behavior->getEMFLevel(EMFLevel::throw_));
    emitter->timeoutIn(defaultEmfTimeout);
}

void Ghost::useNode(Node *node) {
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
    // Make it emit emf
    auto emitter = node->GetOrCreateComponent<EMFEmitter>();
    emitter->setLevel(behavior->getEMFLevel(EMFLevel::touch));
    emitter->timeoutIn(defaultEmfTimeout);
}

void Ghost::pushDoor(Door *door) {
    door->impulsePush(rng.GetFloat(0.0f, 0.4f), door->getSmartDir());
    // Make it emit emf
    auto emitter = door->GetNode()->GetChild("Door")->GetOrCreateComponent<EMFEmitter>();
    emitter->setLevel(behavior->getEMFLevel(EMFLevel::touch));
    emitter->timeoutIn(defaultEmfTimeout);
}

float Ghost::getAggression() const {
    return Max(0.01f, (-(float(levelManager->getTeamSanity())-100))/100) * levelManager->getGhostIdentity().agression * behavior->agression;
}

void Ghost::roam(bool far, bool respectTimer) {
    if (currentPath.empty()) {
        if (respectTimer) {
            if (!nextRoamIn) {
                // Reset timer
                roamTimer.Reset();
                nextRoamIn = rng.GetUInt(1000, 7500);
                return;
            } else if (roamTimer.GetMSec(false) < nextRoamIn) {
                return;
            }
        }
        // Get random location around the ghost
        Vector3 nPos;
        unsigned tries = 0;
        do {
            constexpr float maxDist = 5.0f,
                            farMaxDist = 50.0f,
                            minDist = 5.0f;
            nPos = rng.GetVector3(Vector3(minDist, 0.0f, minDist), Vector3(maxDist, 0.0f, far?farMaxDist:maxDist));
            if (rng.GetBool(0.5f)) {
                nPos.x_ = -nPos.x_;
            }
            if (rng.GetBool(0.5f)) {
                nPos.z_ = -nPos.z_;
            }
            // Make sure to not get stuck
            if (tries++ == 10000) {
                return;
            }
            // Go there
        } while (!walkTo(GetNode()->GetWorldPosition() + nPos) || (currentPath.size() > 8 && !far));
        nextRoamIn = 0;
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

        // Skip waypoints at ghost position
        if (nextWaypoint == body->GetPosition()) {
            currentPath.pop_front();
            return;
        }

#       ifndef NDEBUG
        // Show debug waypoint marker (slow)
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

        // Get distance and move speed
        float speed = behavior->getCurrentSpeed();
        float distance = (body->GetPosition() - nextWaypoint).Length();
        //speed = Min(speed, distance);

        // Remove waypoint if it was reached or timeout teleport
        if (distance < 0.1f) {
            navigationTimer.Reset();
            currentPath.pop_front();
            return;
        } else if (navigationTimer.GetMSec(false) > 25000.f) {
            body->SetPosition(nextWaypoint);
            navigationTimer.Reset();
            currentPath.pop_front();
            return;
        }

        // Rotate toward next waypoint to reach and move
        GetNode()->LookAt(nextWaypoint, Vector3::UP);
        body->SetLinearVelocity(GetNode()->GetWorldDirection() * speed);

        // Play footstep
        playFootstep();
    } else {
        body->SetLinearVelocity(Vector3::ZERO);
    }
}

void Ghost::playFootstep() {
    if (footstepTimer.GetMSec(false) > -Min(behavior->getCurrentSpeed()*0.75f - 2.0f, -0.3f) * 1000.0f) {
        feet->SetFarDistance(behavior->footstepRange);
        feet->SetGain(0.5f);
        feet->Play(getMiscSFX(context_, rng, "ghost"));
        footstepTimer.Reset();
    }
}

bool Ghost::canSeePlayer(PlayerWDistance player, bool includeElectronics) {
    // Check that player has a value
    if (!player.hasValue()) {
        return false;
    }
    // Check that player is inside house
    if (!player.player->isInsideHouse()) {
        return false;
    }
    // Check if player is in line of sight
    auto vectorToPlayer = player.player->getHead()->GetWorldPosition() - GetNode()->GetWorldPosition();
    ea::vector<PhysicsRaycastResult> hits;
    physicsWorld->Raycast(hits, Ray(GetNode()->GetWorldPosition(), vectorToPlayer.Normalized()), vectorToPlayer.Length());
    if (hits.size() <= 1) {
        return true;
    }
    // Check if player has electronics turned on
    if (includeElectronics && player.distance < behavior->electricVisionRange) {
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

EMFEmitter *Ghost::spawnEMFSpot() {
    return EMFEmitter::spawnSpot(GetScene(), GetNode()->GetWorldPosition());
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
