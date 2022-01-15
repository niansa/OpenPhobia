#include "Ghost.hpp"
#include "Useable.hpp"
#include "EMFEmitter.hpp"
#include "../SphereCastMultiple.hpp"
#include "../LevelManager.hpp"

#include <Urho3D/Math/Ray.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Math/RandomEngine.h>



namespace Game {
void Ghost::FixedUpdate(float timeStep) {
    if (rng.GetBool(0.0025f)) {
        // Get all bodies nearby
        eastl::vector<PhysicsRaycastResult> results;
        constexpr float range = 0.5f;
        SphereCastMultiple(GetNode()->GetComponent<RigidBody>()->GetPhysicsWorld(), results, Ray(GetNode()->GetWorldPosition(), GetNode()->GetWorldDirection()), range, range);
        // Throw an object
        for (unsigned triesLeft = 25; triesLeft != 0; triesLeft--) {
            auto body = results[rng.GetInt(0, results.size()-1)].body_;
            if (body) {
                if (body->GetNode()->HasTag("GhostInteractable") && body->GetNode()->GetParent()->GetName() != "Hand") {
                    throwBody(body);
                    break;
                }
            }
        }
    }
}

void Ghost::throwBody(RigidBody *body) {
    body->SetKinematic(false);
    auto dir = rng.GetDirectionVector3()*1.5f;
    if (dir.y_ < 0) {
        dir.y_ = -dir.y_;
    }
    body->SetLinearVelocity(dir);
    // Make it emit emf
    auto emitter = body->GetNode()->GetOrCreateComponent<EMFEmitter>();
    emitter->setLevel(EMFLevel::throw_);
    emitter->timeoutIn(20);
}
}
