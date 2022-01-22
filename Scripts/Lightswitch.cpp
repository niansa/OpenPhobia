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
}
