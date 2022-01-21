#include "Lightswitch.hpp"
#include "../LevelManager.hpp"



namespace Game {
void Lightswitch::Start() {
    for (auto node : GetNode()->GetParent()->GetChildren(true)) {
        if (node->GetName() != "Lightbulb") {
            continue;
        }
        lightBulbs.push_back(node->GetComponent<Light>());
    }
    if (GetNode()->HasTag("DefaultOn")) {
        turnedOn = true;
        TurnOn();
    } else {
        TurnOff();
    }
}

void Lightswitch::TurnOn() {
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
