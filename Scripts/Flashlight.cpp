#include "Flashlight.hpp"
#include "../LevelManager.hpp"
#include "../SFX.hpp"

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource3D.h>



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
            nextBlinkIn = getGhost()->rng.GetFloat(50.0f, 200.0f);
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
    playClick();
}

void Flashlight::TurnOff() {
    for (const auto lightBulb : GetNode()->GetChildren()) {
        if (lightBulb->GetName() == "Lightbulb") {
            lightBulb->GetComponent<Light>()->SetEnabled(false);
        }
    }
    turnedOn = false;
    playClick();
}

void Flashlight::playClick() {
    auto clickSound = GetNode()->CreateComponent<SoundSource3D>();
    clickSound->SetFarDistance(5.0f);
    clickSound->SetNearDistance(0.2f);
    clickSound->SetAutoRemoveMode(AutoRemoveMode::REMOVE_COMPONENT);
    clickSound->Play(getClickSFX(context_, getLevelManager()->getRng(), "flashlight"));
}
}
