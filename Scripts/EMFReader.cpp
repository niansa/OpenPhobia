#include "EMFReader.hpp"
#include "EMFEmitter.hpp"
#include "../SphereCastMultiple.hpp"
#include "../LevelManager.hpp"

#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Math/Ray.h>



namespace Game {
void EMFReader::Start() {
    leds = GetNode()->GetChild("LEDs");
    TurnOff();
}

void EMFReader::FixedUpdate(float) {
    if (turnedOn) {
        unsigned emfLevel = 1;
        // Get objects nearby
        eastl::vector<PhysicsRaycastResult> result;
        constexpr float range = 1.f;
        SphereCastMultiple(GetNode()->GetComponent<RigidBody>()->GetPhysicsWorld(), result, Ray(GetNode()->GetWorldPosition(), GetNode()->GetWorldDirection()), range, range);
        SphereCastMultiple(GetNode()->GetComponent<RigidBody>()->GetPhysicsWorld(), result, Ray(GetNode()->GetWorldPosition(), - GetNode()->GetWorldDirection()), range/1.5, range/1.5);
        // Find emf emitters
        for (const auto& body : result) {
            if (body.body_) {
                auto node = body.body_->GetNode();
                if (node->HasComponent<EMFEmitter>()) {
                    auto emitter = node->GetComponent<EMFEmitter>();
                    emfLevel = Max(emfLevel, emitter->getLevel());
                }
            }
        }
        // Set final level
        setLevel(emfLevel);
    }
}

void EMFReader::TurnOn() {
    leds->GetChild(unsigned(0))->SetEnabled(true);
}

void EMFReader::TurnOff() {
    for (const auto led : leds->GetChildren()) {
        led->SetEnabled(false);
    }
}

void EMFReader::setLevel(uint8_t level) {
    TurnOff();
    for (unsigned led = 0; led != level; led++) {
        leds->GetChild(led)->SetEnabled(true);
    }
}
}