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
    // Get current rotation
    currentRotation = GetNode()->GetRotation().EulerAngles();
    // Get base angle
    baseAngle = getAngle();
    // Get constraint min/max angle
    auto maxAngleVar = GetNode()->GetVar("maxAngle");
    if (maxAngleVar.IsEmpty()) {
        maxAngle = 90.0f;
    } else {
        maxAngle = maxAngleVar.GetFloat();
    }
    // Get if direction needs to be negative
    //negDir = GetNode()->HasTag("negativeDoor");
    // Push door by random value
    impulsePush(rng.GetFloat(0.0f, maxAngle));
}

void Door::FixedUpdate(float) {
    if (impulse > impulseSpeed) {
        push(impulseSpeed);
        impulse -= impulseSpeed;
    }
}

float Door::getRelativeAngle() const {
    return fabsf(getAngle()-baseAngle);
}

bool Door::getSmartDir() const {
    return getRelativeAngle() > getRelativeFullyOpenAngle() / 2;
}

void Door::push(float power) {
    if (!(power > 0.0f && isFullyOpen()) && !(power < 0.0f && isClosed())) {
        currentRotation.y_ = Min(getFullyOpenAngle(), currentRotation.y_+power);
        GetNode()->SetRotation(Quaternion(currentRotation));
    }
}
void Door::push(float power, bool direction) {
    push(direction?-power:power);
}

void Door::impulsePush(float power) {
    if (!(power > 0.0f && isFullyOpen()) && !(power < 0.0f && isClosed())) {
        impulse += power;
    }
}
void Door::impulsePush(float power, bool direction) {
    impulsePush(direction?-power:power);
}
}
