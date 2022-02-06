#include "Door.hpp"
#include "../LevelManager.hpp"
#include "EMFEmitter.hpp"

#include <Urho3D/Scene/Node.h>
#include <Urho3D/Physics/RigidBody.h>



namespace Game {
void Door::Start() {
    // Get door node
    doorNode = GetNode()->GetChild("RotOff")->GetChild("Door");
    // Get door body
    doorBody = doorNode->GetComponent<RigidBody>();
    // Set the right collision layer and mask
    doorBody->SetCollisionLayerAndMask(1, 1);
}

void Door::FixedUpdate(float) {
    switch (state) {
    case opening: {
        auto cRot = doorNode->GetRotation().EulerAngles();
        cRot.y_ = Max(-maxAngle, cRot.y_ - moveSpeed);
        doorNode->SetRotation(Quaternion(cRot));
        if (cRot.y_ == -maxAngle) {
            setState(still);
            forced = false;
        }
    } break;
    case closing: {
        auto cRot = doorNode->GetRotation().EulerAngles();
        cRot.y_ = Min(0, cRot.y_ + moveSpeed);
        doorNode->SetRotation(Quaternion(cRot));
        if (cRot.y_ == 0) {
            setState(still);
            forced = false;
        }
    } break;
    default: {}
    }
}

void Door::setState(State nState) {
    state = nState;
    doorBody->SetTrigger(state == opening || state == closing);
}

void Door::Activate() {
    // Don't interrupt if forced
    if (forced) {
        return;
    }
    // Get direction to go
    if (state == still) {
        setState((-doorNode->GetRotation().EulerAngles().y_ > maxAngle / 2)?closing:opening);
    } else {
        setState(still);
    }
}
}
