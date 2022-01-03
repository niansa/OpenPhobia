#include "Lightswitch.hpp"
#include "../LevelManager.hpp"



namespace Game {
void Lightswitch::Start() {
    lightBulb = GetNode()->GetParent()->GetChild("Lightbulb")->GetComponent<Light>();
    TurnOff();
}

void Lightswitch::TurnOn() {
    //GetNode()->SetRotation(Quaternion(Vector3{0, -90, 0}));
    lightBulb->SetEnabled(true);
}

void Lightswitch::TurnOff() {
    lightBulb->SetEnabled(false);
    //GetNode()->SetRotation(Quaternion(Vector3{0, -90, 180}));
}
}
