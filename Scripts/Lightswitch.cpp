#include "Lightswitch.hpp"
#include "../LevelManager.hpp"



namespace Game {
void Lightswitch::Start() {
    // Set ghost use cooldown
    ghostUseCooldown = 100;
    // Find all
    for (auto node : GetNode()->GetParent()->GetChildren(true)) {
        if (node->GetName() != "Lightbulb") {
            continue;
        }
        lightBulbs.push_back(node->GetComponent<Light>());
    }
    // Turn on light if turned on by default
    if (GetNode()->HasTag("DefaultOn")) {
        turnedOn = true;
        TurnOn();
    } else {
        TurnOff();
    }
    // Initially the ghost should be present... I guess
    ghostyDim(true);
}

void Lightswitch::TurnOn() {
    cooldown.Reset();
    for (auto lightBulb : lightBulbs) {
        //GetNode()->SetRotation(Quaternion(Vector3{0, -90, 0}));
        lightBulb->SetEnabled(true);
    }
}

void Lightswitch::TurnOff() {
    for (auto lightBulb : lightBulbs) {
        lightBulb->SetEnabled(false);
        //GetNode()->SetRotation(Quaternion(Vector3{0, -90, 180}));
    }
}

void Lightswitch::ghostyDim(bool enable) {
    constexpr float dim = 0.75f;
    if (enable) {
        for (auto lightBulb : lightBulbs) {
            lightBulb->SetBrightness(lightBulb->GetBrightness()*dim);
        }
    } else {
        for (auto lightBulb : lightBulbs) {
            lightBulb->SetBrightness(lightBulb->GetBrightness()/dim);
        }
    }
}
}
