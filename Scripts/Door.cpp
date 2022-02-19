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
    push(rng.GetFloat(0.0f, 10.0f));
}

void Door::FixedUpdate(float) {
    // Snap door in place when closed
    if (getRelativeAngle() < 0.05f) {
        //doorBody->SetKinematic(true);
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

void Door::push(float power) {
    //doorBody->SetKinematic(false);
    if (negDir) {
        power = -power;
    }
    doorBody->ApplyTorque({0.0f, power, 0.0f});
}

void Door::pushAutoDir(float power) {
    auto currentAngle = getRelativeAngle();
    auto openAngle = getRelativeOpenAngle();
    // Get direction
    if (currentAngle < openAngle / 2) {
        power = -power;
    }
    // Push
    push(power);
}
}
