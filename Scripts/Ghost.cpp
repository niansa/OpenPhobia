#include "Ghost.hpp"
#include "Useable.hpp"
#include "EMFEmitter.hpp"
#include "../SphereCastMultiple.hpp"
#include "../LevelManager.hpp"

#include <Urho3D/Math/Ray.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Math/RandomEngine.h>
#include <Urho3D/Physics/KinematicCharacterController.h>



namespace Game {
void Ghost::Start() {
    // Create kinematic controller
    kinematicController = GetNode()->CreateComponent<KinematicCharacterController>();
    kinematicController->SetCollisionLayerAndMask(10, 1);
    // Find physics world
    for (const auto node : GetScene()->GetChildren(true)) {
        if (node->HasComponent<RigidBody>()) {
            physicsWorld = node->GetComponent<RigidBody>()->GetPhysicsWorld();
            break;
        }
    }
    // Set initial ghost state
    setState(GhostState::local);
}

void Ghost::FixedUpdate(float timeStep) {
    // Interact
    if (rng.GetBool(0.0025f)) {
        // Get all bodies nearby
        eastl::vector<PhysicsRaycastResult> results;
        constexpr float range = 0.5f;
        SphereCastMultipleDS(physicsWorld, results, Ray(GetNode()->GetWorldPosition(), GetNode()->GetWorldDirection()), range, range);
        // Throw an object
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
    // State-dependent
    if (stepTimer.GetMSec(false) > 200) {
        switch (state) {
        case GhostState::local: {
            // Consider changing state
            if (rng.GetBool(0.05f)) {
                state = GhostState::roaming;
            }
        } break;
        case GhostState::roaming: {
            // Roam around
            if (rng.GetBool(0.5f)) {
                kinematicController->SetWalkDirection(GetNode()->GetWorldDirection()*rng.GetFloat(0.f, 0.04f));
            }
            if (rng.GetBool(0.1f)) {
                GetNode()->Rotate(Quaternion(Vector3{0, float(rng.GetInt(-10, rng.GetBool(0.25f)?180:90)), 0}));
            }
            // Consider changing state
            if (rng.GetBool(0.075f)) {
                setState(GhostState::local);
            } else {
                // Consider teleporting to waypoint
            }
        } break;
        }
        stepTimer.Reset();
    }
}

void Ghost::setState(GhostState nState) {
    state = nState;
    stateTimer.Reset();
    kinematicController->SetWalkDirection(Vector3::ZERO);
}

void Ghost::throwBody(RigidBody *body) {
    body->SetKinematic(false);
    // Get random direction
    auto dir = rng.GetDirectionVector3()*1.5f;
    if (dir.y_ < 0) {
        dir.y_ = -dir.y_;
    }
    // Get random power
    auto power = rng.GetFloat(0.25f, 1.75f);
    body->SetLinearVelocity(dir*power);
    // Make it emit emf
    auto emitter = body->GetNode()->GetOrCreateComponent<EMFEmitter>();
    emitter->setLevel(EMFLevel::throw_);
    emitter->timeoutIn(20);
}

void Ghost::useBody(RigidBody *body) {
    auto node = body->GetNode();
    auto script = static_cast<Useable *>(node->GetComponent(node->GetName()));
    script->Use();
}
}
