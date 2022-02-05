#include "Ghost.hpp"
#include "Player.hpp"
#include "Useable.hpp"
#include "EMFEmitter.hpp"
#include "Lightswitch.hpp"
#include "GhostBehavior.hpp"
#include "RoomBoundary.hpp"
#include "../SphereCastMultiple.hpp"
#include "../LevelManager.hpp"

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
    // Set initial ghost state
    setState(GhostState::local);
    // Start its animation
    animationController->Play("Objects/Ghost/Animations/Idle.ani", 1, true);
}

void Ghost::FixedUpdate(float) {
    // Keep following the currentPath
    followPath();
    // Interact
    if (isVisible() || rng.GetBool(0.0025f * getAggression())) {
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
                        break;
                    }
                }
            }
        }
    }
#   ifndef NDEBUG
    // Debug keys
    auto input = GetSubsystem<Input>();
    if (input->GetKeyDown(Key::KEY_M)) {
        setState(GhostState::roaming);
    } else if (input->GetKeyDown(Key::KEY_R)) {
        setState(GhostState::reveal);
    } else if (input->GetKeyDown(Key::KEY_L)) {
        setState(GhostState::local);
    } else if (input->GetKeyDown(Key::KEY_H)) {
        setState(GhostState::hunt);
    }
#   endif
    // Low frequency timed clode
    if (lowFreqStepTimer.GetMSec(false) > 2500) {
        // Update list of close objects
        updateCloseBodies();
        // Reset step timer
        lowFreqStepTimer.Reset();
    }
    // Timed code
    if (stepTimer.GetMSec(false) > 200) {
        // Update closest player
        updateClosestPlayer();
        // State-dependent code
        switch (state) {
        case GhostState::local: {
            if (currentPath.empty()) {
                auto cRoom = getCurrentRoom();
                if (cRoom) {
                    // Go to random location inside of the room
                    walkTo(rng.GetVector3(cRoom->getBoundingBox()));
                }
            }
        } break;
        case GhostState::hunt: {
            // Switch blink
            if (blinkTimer.GetMSec(false) > nextBlinkIn) {
                appearance->SetDeepEnabled(!appearance->IsEnabled());
                nextBlinkIn = behavior->getBlinkSpeed();
                blinkTimer.Reset();
            }
            // Use last hunt timer as grace period timer
            if (lastHuntTimer.GetMSec(false) < behavior->gracePeriod * 1000.0f) {
                break;
            }
            // Get player to chase
            auto playerToChase = behavior->getPlayerToChase();
            // Kill player if possible
            if (playerToChase.distance < 0.75f) {
                playerToChase.player->startKillingPlayer();
                setState(behavior->endHuntOnDeath?GhostState::local:GhostState::hunt);
            }
            // Chase player if possible
            if (canSeePlayer(playerToChase)) {
                if (chasePlayer()) {
                    break;
                }
            }
        }
        case GhostState::roaming: {
            if (currentPath.empty()) {
                // Get random location around the ghost
                Vector3 nPos;
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
                    // Go there
                } while (!walkTo(GetNode()->GetWorldPosition() + nPos));
            }
        } break;
        default: {}
        }
        // Potential state switch
        trySwitchState();
        // Reset step timer
        stepTimer.Reset();
    }
}

void Ghost::setState(GhostState nState) {
    // Backup old state
    auto oldState = state;
    // Set new state
    state = nState;
    // Stop walking
    currentPath.clear();
    // Set ghost visibility
    appearance->SetDeepEnabled(state == GhostState::reveal || state == GhostState::hunt);

    // Check that behavior has loaded
    if (behavior) {
        // State dependent code
        switch (state) {
        case GhostState::local: {
            // Handle previous state
            if (oldState == GhostState::hunt) {
                // Reset hunt timer
                lastHuntTimer.Reset();
                // Stop hunt sound
                GetNode()->GetComponent<SoundSource3D>()->Stop();
                // Warp back home
                kinematicController->Warp(homePosition);
            } else {
                // Go back home
                walkTo(homePosition);
            }
            // Find and define next state
            GhostState nState;
            auto teamSanity = levelManager->getTeamSanity();
            if (teamSanity < behavior->sanityThreshold + behavior->getHuntMultiplier() && lastHuntTimer.GetMSec(false) > (behavior->huntCooldown - 1) * 1000 && rng.GetBool(teamSanity>25?0.25f:0.5f)) {
                nState = GhostState::hunt;
            } else if (rng.GetBool(0.025f*getAggression())) {
                nState = GhostState::reveal;
            } else  {
                nState = GhostState::roaming;
            }
            setNextState(nState, rng.GetFloat(1000, 1000/getAggression()));
        } break;
        case GhostState::roaming: {
            setNextState(GhostState::local, rng.GetFloat(5000, Min(20000*getAggression(), 5000)));
        } break;
        case GhostState::reveal: {
            appearance->SetDeepEnabled(true);
            setNextState(GhostState::local, rng.GetFloat(2500, Min(15000*getAggression(), 1500)));
        } break;
        case GhostState::hunt: {
            // Check that hunt is allowed to start
            if (!levelManager->isAnyPlayerInHouse()) {
                setState(GhostState::local);
                break;
            }
            // Reset stuff
            if (oldState != GhostState::hunt) {
                lastHuntTimer.Reset();
                behavior->onHuntStart();
                // Play hunt sound TODO: Fixup
                auto huntSound = GetNode()->GetOrCreateComponent<SoundSource3D>();
                huntSound->SetFarDistance(behavior->vocalRange);
                huntSound->SetNearDistance(1.0f);
                huntSound->Play(GetSubsystem<ResourceCache>()->GetResource<Sound>("SFX/ghostSingMale.ogg"));
            }
            setNextState(GhostState::local, behavior->huntDuration*1000.0f);
        } break;
        default: {}
        }
    } else {
        // Behavior is not loaded - just start roaming after 15 seconds
        setNextState(GhostState::roaming, 15000.0f);
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
    emitter->timeoutIn(15);
}

float Ghost::getAggression() const {
    return Max(0.01f, (-(float(levelManager->getTeamSanity())-100))/100) * baseAgression * behavior->agression;
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

bool Ghost::chasePlayer() {
    auto [player, playerDistance] = behavior->getPlayerToChase();
    return player?walkTo(player->GetNode()->GetWorldPosition()):false;
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
}
