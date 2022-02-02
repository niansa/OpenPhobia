#include "Lightswitch.hpp"
#include "Lightbulb.hpp"
#include "Ghost.hpp"

#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>



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
    if (levelManager->getGhost()->getState() != GhostState::hunt) {
        forceTurnOn();
    }
}

void Lightswitch::TurnOff() {
    if (!levelManager->isGhostLoaded() || levelManager->getGhost()->getState() != GhostState::hunt) {
        forceTurnOff();
    }
}

void Lightswitch::GhostTurnOn() {
    forceTurnOn();
}

void Lightswitch::GhostTurnOff() {
    forceTurnOff();
}

void Lightswitch::forceTurnOn() {
    for (auto lightBulb : lightBulbs) {
        lightBulb->TurnOn();
    }
    turnedOn = true;
}

void Lightswitch::forceTurnOff() {
    for (auto lightBulb : lightBulbs) {
        lightBulb->TurnOff();
    }
    turnedOn = false;
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
}
