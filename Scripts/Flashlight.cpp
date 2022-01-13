#include "Flashlight.hpp"
#include "../LevelManager.hpp"



namespace Game {
void Flashlight::Start() {
    TurnOff();
}

void Flashlight::TurnOn() {
    for (const auto lightBulb : GetNode()->GetChildren()) {
        if (lightBulb->GetName() == "Lightbulb") {
            lightBulb->GetComponent<Light>()->SetEnabled(true);
        }
    }
}

void Flashlight::TurnOff() {
    for (const auto lightBulb : GetNode()->GetChildren()) {
        if (lightBulb->GetName() == "Lightbulb") {
            lightBulb->GetComponent<Light>()->SetEnabled(false);
        }
    }
}
}
