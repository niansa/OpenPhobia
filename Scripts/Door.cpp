#include "Door.hpp"
#include "../LevelManager.hpp"
#include "EMFEmitter.hpp"

#include <Urho3D/Scene/Node.h>
#include <Urho3D/Physics/RigidBody.h>



namespace Game {
void Door::Start() {
    // Get rng from LevelManager
    auto& rng = GetGlobalVar("LevelManager").GetCustom<LevelManager*>()->getRng();
    // Get door node
    doorNode = GetNode()->GetChild("Door");
    // Get door body
    doorBody = doorNode->GetComponent<RigidBody>();
    // Make door body kinematic
    //doorBody->SetKinematic(true);
    // Set the right collision layer and mask
    doorBody->SetCollisionLayerAndMask(1, 1);
    // Push door by random value
    push(rng.GetFloat(0.0f, 10.0f));
}

void Door::FixedUpdate(float) {
    // Snap door in place when closed
    auto a = (doorBody->GetRotation() - GetNode()->GetWorldRotation()).EulerAngles();
    if (a.y_ < 0.2f) {
        doorBody->SetAngularVelocity(Vector3::ZERO);
        doorBody->SetLinearVelocity(Vector3::ZERO);
    }
}

void Door::push(float power) {
    doorBody->ApplyTorque({0.0f, -power, 0.0f});
}

void Door::push(const Vector3& direction, float power) {
    doorBody->ApplyForce(direction*power);
}
}
