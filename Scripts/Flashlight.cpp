#include "Flashlight.hpp"
#include "../LevelManager.hpp"



namespace Game {
void Flashlight::Start() {
    lightBulb = GetNode()->GetChild("Lightbulb")->GetComponent<Light>();
    TurnOff();
}

void Flashlight::TurnOn() {
    lightBulb->SetEnabled(true);
}

void Flashlight::TurnOff() {
    lightBulb->SetEnabled(false);
}
}
