#include "Ghost.hpp"
#include "Player.hpp"
#include "Useable.hpp"
#include "EMFEmitter.hpp"
#include "Lightswitch.hpp"
#include "../SphereCastMultiple.hpp"
#include "../LevelManager.hpp"

#include <Urho3D/Math/Ray.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Math/RandomEngine.h>
#include <Urho3D/Physics/KinematicCharacterController.h>
#include <Urho3D/Navigation/DynamicNavigationMesh.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/Material.h>
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
    for (const auto node : GetScene()->GetChildren(true)) {
        if (node->HasComponent<RigidBody>()) {
            physicsWorld = node->GetComponent<RigidBody>()->GetPhysicsWorld();
            break;
        }
    }
    // Get random ghost appearance
    appearanceInfo = &(ghostAppearances[rng.GetUInt(0, ghostAppearances.size())]);
    // Set ghost appearance
    eastl::vector<AnimatedModel*> models;
    appearance->GetChild("Model")->GetComponents<AnimatedModel>(models);
    for (auto model : models) {
        Material *mat = model->GetMaterial();
        mat->SetShaderParameter("MatEmissiveColor", Variant(appearanceInfo->color));
    }
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
        // Get all bodies nearby
        eastl::vector<PhysicsRaycastResult> results;
        constexpr float range = 1.0f;
        SphereCastMultipleDS(physicsWorld, results, Ray(GetNode()->GetWorldPosition(), GetNode()->GetWorldDirection()), range, range);
        // Throw or use an object
        if (!results.empty()) {
            for (unsigned triesLeft = 25; triesLeft != 0; triesLeft--) {
                auto body = results[rng.GetInt(0, results.size()-1)].body_;
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
    // Timed state-dependent code
    if (stepTimer.GetMSec(false) > 200) {
        // State-dependent code
        switch (state) {
        case GhostState::roaming: {
            // Roam around  TODO: Use navigation
            /*if (rng.GetBool(0.5f)) {
                kinematicController->SetWalkDirection(GetNode()->GetWorldDirection()*rng.GetFloat(0.f, 0.04f));
            }
            if (rng.GetBool(0.1f)) {
                GetNode()->Rotate(Quaternion(Vector3{0, float(rng.GetInt(-10, rng.GetBool(0.25f)?180:90)), 0}));
            }*/
        } break;
        case GhostState::hunt: {
            if (currentPath.empty()) {
                chaseNearestPlayer();
            }
        } break;
        default: {}
        }
        // Reset step timer
        stepTimer.Reset();
        // Potential state switch
        trySwitchState();
    }
}

void Ghost::setState(GhostState nState) {
    if (state == GhostState::hunt) {
        lastHuntTimer.Reset();
    }
    // Set new state
    state = nState;
    // Stop walking
    currentPath.clear();
    // Set ghost visibility
    appearance->SetDeepEnabled(state == GhostState::reveal || state == GhostState::hunt);
    // State dependent code
    switch (state) {
    case GhostState::local: {
        GhostState nState;
        if (levelManager->getTeamSanity() < behavior.sanityThreshold && lastHuntTimer.GetMSec(false) > behavior.huntCooldown * 1000 &&rng.GetBool(0.5f/*TODO: this value should be dynamic*/)) {
            nState = GhostState::hunt;
        } else if (rng.GetBool(0.025f*getAggression())) {
            nState = GhostState::reveal;
        } else  {
            nState = GhostState::roaming;
        }
        setNextState(nState, rng.GetFloat(1000, 1000/getAggression()));
    } break;
    case GhostState::roaming: {
        setNextState(GhostState::local, rng.GetFloat(5000, 20000*getAggression()));
    } break;
    case GhostState::reveal: {
        setNextState(GhostState::local, rng.GetFloat(2500, 15000*getAggression()));
    } break;
    case GhostState::hunt: {
        setNextState(GhostState::local, behavior.huntDuration*1000.0f);
        chaseNearestPlayer();
    } break;
    default: {}
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
    auto power = getAggression()>0.5f?rng.GetFloat(0.75f, 2.25f):rng.GetFloat(0.25f, 1.75f);
    body->SetLinearVelocity(dir*power);
    // Make it emit emf
    auto emitter = body->GetNode()->GetOrCreateComponent<EMFEmitter>();
    emitter->setLevel(EMFLevel::throw_);
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
    // Make it emit emf
    auto emitter = body->GetNode()->GetOrCreateComponent<EMFEmitter>();
    emitter->setLevel(EMFLevel::touch);
    emitter->timeoutIn(15);
}

float Ghost::getAggression() const {
    return Max(0.01f, (-(float(levelManager->getTeamSanity())-100))/100) * baseAgression;
}

eastl::tuple<Player*, float> Ghost::getPlayerToChase() {
    // Get nearest player
    Player *closestPlayer = nullptr;
    float closestPlayerDistance = 100.f;
    for (auto player : levelManager->getPlayers()) {
        auto distance = (GetNode()->GetWorldPosition() - player->GetNode()->GetWorldPosition()).Length();
        if (distance < closestPlayerDistance) {
            closestPlayerDistance = distance;
            closestPlayer = player;
        }
    }
    return {closestPlayer, closestPlayerDistance};
}

void Ghost::chaseNearestPlayer() {
    auto [player, playerDistance] = getPlayerToChase();
    navMesh->FindPath(currentPath, GetNode()->GetWorldPosition(), player->GetNode()->GetWorldPosition());
}

void Ghost::followPath() {
    if (!currentPath.empty()) {
        auto nextWaypoint = currentPath[0];
        nextWaypoint.y_ = GetNode()->GetWorldPosition().y_;

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

        // Get distance and stuff
        float move = 0.8f / 60.f;
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

        // Rotate toward next waypoint to reach and move without overshooting the target
        GetNode()->LookAt(nextWaypoint, Vector3::UP);
        kinematicController->SetWalkDirection(GetNode()->GetWorldDirection() * move);
    } else {
        kinematicController->SetWalkDirection(Vector3::ZERO);
    }
}
}
