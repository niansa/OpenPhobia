#include "Flashlight.hpp"
#include "../LevelManager.hpp"



namespace Game {
void Flashlight::Start() {
    TurnOff();
}

void Flashlight::Update(float) {
    if (turnedOn && isDistorted()) {
        if (blinkTimer.GetMSec(false) > nextBlinkIn) {
            float bNes = NAN;
            for (const auto lightBulb : GetNode()->GetChildren()) {
                if (lightBulb->GetName() == "Lightbulb") {
                    auto light = lightBulb->GetComponent<Light>();
                    if (isnan(bNes)) {
                        bNes = light->GetBrightness();
                        bNes = bNes>0.5?getGhost()->rng.GetFloat(0.1f, 0.3f):getGhost()->rng.GetFloat(0.65f, 0.8f);
                    }
                    light->SetBrightness(bNes);
                }
            }
            nextBlinkIn = getGhost()->getGhostBehavior()->getFlashlightBlinkSpeed();
            blinkTimer.Reset();
        }
    } else {
        for (const auto lightBulb : GetNode()->GetChildren()) {
            if (lightBulb->GetName() == "Lightbulb") {
                lightBulb->GetComponent<Light>()->SetBrightness(1.0f);
            }
        }
    }
}

void Flashlight::TurnOn() {
    for (const auto lightBulb : GetNode()->GetChildren()) {
        if (lightBulb->GetName() == "Lightbulb") {
            lightBulb->GetComponent<Light>()->SetEnabled(true);
        }
    }
    turnedOn = true;
}

void Flashlight::TurnOff() {
    for (const auto lightBulb : GetNode()->GetChildren()) {
        if (lightBulb->GetName() == "Lightbulb") {
            lightBulb->GetComponent<Light>()->SetEnabled(false);
        }
    }
    turnedOn = false;
}
}
