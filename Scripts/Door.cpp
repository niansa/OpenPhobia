#include "Door.hpp"
#include "../LevelManager.hpp"
#include "EMFEmitter.hpp"

#include <Urho3D/Scene/Node.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/Constraint.h>



namespace Game {
void Door::Start() {
    // Get rng from LevelManager
    auto& rng = GetGlobalVar("LevelManager").GetCustom<LevelManager*>()->getRng();
    // Get door node
    doorNode = GetNode()->GetChild("Door");
    // Get door body
    doorBody = doorNode->GetComponent<RigidBody>();
    // Set damping
    doorBody->SetAngularDamping(0.8f);
    doorBody->SetLinearDamping(0.8f);
    // Get constraint min/max angle
    auto constraint = GetNode()->GetChild("Hinge")->GetComponent<Constraint>();
    minAngle = constraint->GetLowLimit().x_;
    maxAngle = constraint->GetHighLimit().x_;
    // Get if direction needs to be negative
    negDir = fabsf(maxAngle) < fabsf(minAngle);
    // Set constraint target node
    constraint->SetOtherBody(doorBody);
    // Set the right collision layer and mask
    doorBody->SetCollisionLayerAndMask(1, 1);
    // Push door by random value
    impulsePush(rng.GetFloat(0.0f, 2.5f));
}

void Door::FixedUpdate(float) {
    // Snap door in place when closed
    if (isClosed()) {
        doorBody->SetAngularVelocity(Vector3::ZERO);
        doorBody->SetLinearVelocity(Vector3::ZERO);
    }
}

float Door::getAngle() const {
    return (doorBody->GetRotation().EulerAngles().y_ - GetNode()->GetWorldRotation().EulerAngles().y_);
}

float Door::getRelativeAngle() const {
    return fabsf(getAngle()-getClosedAngle());
}

float Door::getRelativeOpenAngle() const {
    return fabsf(minAngle-maxAngle);
}

bool Door::getSmartDir() const {
    return getRelativeAngle() < getRelativeOpenAngle() / 2;
}

void Door::push(float power, bool direction) {
    if (negDir) {
        power = -power;
    }
    if (direction) {
        power = -power;
    }
    doorBody->ApplyTorque({0.0f, power, 0.0f});
}

void Door::impulsePush(float power, bool direction) {
    if (negDir) {
        power = -power;
    }
    if (direction) {
        power = -power;
    }
    doorBody->ApplyTorqueImpulse({0.0f, power, 0.0f});
}
}
