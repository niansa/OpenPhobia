#include "GhostHunt.hpp"
#include "Ghost.hpp"
#include "Player.hpp"

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource3D.h>



namespace Game {
void GhostHunt::Initialize() {
    // Check that hunt is allowed to start
    if (!GetGhost()->levelManager->isAnyPlayerInHouse()) {
        GetGhost()->setState("Local");
        return;
    }
    // Get visible
    GetGhost()->appearance->SetDeepEnabled(true);
    // Play hunt sound
    auto sound = GetNode()->GetOrCreateComponent<SoundSource3D>();
    sound->SetFarDistance(GetGhost()->behavior->vocalRange);
    sound->Play(GetSubsystem<ResourceCache>()->GetResource<Sound>("SFX/ghostSingMale.ogg"));
    restart();
}

void GhostHunt::Deinitialize() {
    GetGhost()->lastHuntTimer.Reset();
    // Stop vocal sound
    GetNode()->GetComponent<SoundSource3D>()->Stop();
    // Reset hunt timer
    GetGhost()->lastHuntTimer.Reset();
    // Warp back home
    GetGhost()->kinematicController->Warp(GetGhost()->homePosition);
}

void GhostHunt::FixedUpdate(float) {
    // Switch blink
    if (blinkTimer.GetMSec(false) > nextBlinkIn) {
        GetGhost()->appearance->SetDeepEnabled(!GetGhost()->appearance->IsEnabled());
        nextBlinkIn = GetGhost()->behavior->getBlinkSpeed();
        blinkTimer.Reset();
    }
    // Use last hunt timer as grace period timer
    if (GetGhost()->lastHuntTimer.GetMSec(false) < GetGhost()->behavior->gracePeriod) {
        return;
    }
    // Don't run this piece of code as often
    if (stepTimer.GetMSec(false) > 250) {
        stepTimer.Reset();
        // Get player to chase
        auto playerToChase = GetGhost()->behavior->getPlayerToChase();
        if (playerToChase.hasValue()) {
            // Kill player if possible
            if (playerToChase.distance < 0.75f) {
                playerToChase.player->startKillingPlayer();
                if (GetGhost()->behavior->endHuntOnDeath) {
                    GetGhost()->setState("Local");
                } else {
                    restart();
                }
                return;
            }
            // Chase player if possible
            if (GetGhost()->canSeePlayer(playerToChase, true)) {
                if (chasePlayer()) {
                    return;
                }
            }
        }
        // Just roam around
        GetGhost()->roam();
    }
}

bool GhostHunt::chasePlayer() {
    auto player = GetGhost()->behavior->getPlayerToChase();
    return player.hasValue()?GetGhost()->walkTo(player.player->GetNode()->GetWorldPosition()):false;
}

void GhostHunt::restart() {
    GetGhost()->setNextState("Local", GetGhost()->behavior->huntDuration);
}
}
