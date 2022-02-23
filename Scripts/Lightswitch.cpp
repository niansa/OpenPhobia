#include "Lightswitch.hpp"
#include "Lightbulb.hpp"
#include "Ghost.hpp"
#include "../SFX.hpp"

#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource3D.h>



namespace Game {
void Lightswitch::Start() {
    // Get level manager
    levelManager = GetGlobalVar("LevelManager").GetCustom<LevelManager*>();
    // Set ghost use cooldown
    ghostUseCooldown = 100;
    // Find all
    for (auto node : GetNode()->GetParent()->GetChildren(true)) {
        if (node->HasComponent<Lightbulb>()) {
            lightBulbs.push_back(node->GetComponent<Lightbulb>());
        }
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
    if (levelManager->getGhost()->getState() != "Hunt") {
        forceTurnOn();
    }
}

void Lightswitch::TurnOff() {
    if (!levelManager->isGhostLoaded() || levelManager->getGhost()->getState() != "Hunt") {
        forceTurnOff();
    }
}

void Lightswitch::forceTurnOn() {
    for (auto lightBulb : lightBulbs) {
        lightBulb->TurnOn();
    }
    turnedOn = true;
    playClick();
}

void Lightswitch::forceTurnOff() {
    for (auto lightBulb : lightBulbs) {
        lightBulb->TurnOff();
    }
    turnedOn = false;
    playClick();
}

void Lightswitch::ghostyDim(bool enable) {
    constexpr float dim = 0.75f;
    if (enable && !ghostyDimmed) {
        for (auto lightBulb : lightBulbs) {
            lightBulb->getLight()->SetBrightness(lightBulb->getLight()->GetBrightness()*dim);
        }
        ghostyDimmed = true;
    } else if (!enable && ghostyDimmed) {
        for (auto lightBulb : lightBulbs) {
            lightBulb->getLight()->SetBrightness(lightBulb->getLight()->GetBrightness()/dim);
        }
        ghostyDimmed = false;
    }
}

void Lightswitch::playClick() {
    auto clickSound = GetNode()->CreateComponent<SoundSource3D>();
    clickSound->SetFarDistance(20.0f);
    clickSound->SetNearDistance(0.5f);
    clickSound->SetAutoRemoveMode(AutoRemoveMode::REMOVE_COMPONENT);
    clickSound->Play(getClickSFX(context_, levelManager->getRng(), "lightswitch"));
}
}
