#include "Lightbulb.hpp"
#include "../LevelManager.hpp"



namespace Game {
void Lightbulb::Start() {
    light = GetNode()->GetComponent<Light>();
    originalBnes = light->GetBrightness();
    TurnOff();
}

void Lightbulb::FixedUpdate(float timeStep) {
    if (fading) {
        // Get current brightness
        auto bnes = light->GetBrightness();
        // Increase/Decrease brightness
        if (turnedOn) {
            bnes += timeStep * 8.0f * originalBnes;
            if (bnes >= originalBnes) {
                bnes = originalBnes;
                fading = false;
            }
        } else {
            bnes -= timeStep * 10.0f * originalBnes;
            if (bnes <= 0.0f) {
                bnes = 0.0f;
                fading = false;
            }
        }
        // Set new brightness
        light->SetBrightness(bnes);
        // If no longer fading, use SetEnabled() for performance
        if (!fading) {
            light->SetEnabled(turnedOn);
        }
    }
}

void Lightbulb::TurnOn() {
    turnedOn = true;
    fading = true;
    light->SetEnabled(true);
}

void Lightbulb::TurnOff() {
    turnedOn = false;
    fading = true;
}
}
