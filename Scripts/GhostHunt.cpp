#include "GhostHunt.hpp"
#include "Ghost.hpp"
#include "Player.hpp"
#include "../SFX.hpp"

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource3D.h>



namespace Game {
void GhostHunt::Initialize() {
    // Check that hunt is allowed to start
    if (!GetGhost()->levelManager->isAnyPlayerInHouse()) {
        aborted = true;
        GetGhost()->setState("Local");
        return;
    }
    // Play hunt sound
    auto sound = GetNode()->GetOrCreateComponent<SoundSource3D>();
    sound->SetFarDistance(GetGhost()->behavior->vocalRange);
    sound->Play(GetGhost()->levelManager->getGhostIdentity().huntSound);
    restart();
}

void GhostHunt::Deinitialize() {
    if (!aborted) {
        GetGhost()->lastHuntTimer.Reset();
        // Stop vocal sound
        GetNode()->GetComponent<SoundSource3D>()->Stop();
        // Reset hunt timer
        GetGhost()->lastHuntTimer.Reset();
        // Warp back home
        GetGhost()->body->SetPosition(GetGhost()->homePosition);
    }
}

void GhostHunt::FixedUpdate(float) {
    // Grace period
    if (gracePeriodTimer.GetMSec(false) < GetGhost()->behavior->gracePeriod) {
        return;
    }
    // DNV: Switch blink
    if (blinkTimer.GetMSec(false) > nextBlinkIn) {
        GetGhost()->appearance->SetDeepEnabled(!GetGhost()->appearance->IsEnabled());
        nextBlinkIn = GetGhost()->behavior->getBlinkSpeed();
        blinkTimer.Reset();
    }
    // Don't run this piece of code as often
    if (stepTimer.GetMSec(false) > 250) {
        stepTimer.Reset();
        // Get player to chase
        auto playerToChase = GetGhost()->behavior->getPlayerToChase();
        if (playerToChase.hasValue()) {
            // Kill player if possible
            if (playerToChase.distance < 1.0f && GetGhost()->canSeePlayer(playerToChase)) {
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
        GetGhost()->markNextRoamAsFar();
        GetGhost()->roam(false);
    }
}

bool GhostHunt::chasePlayer() {
    auto player = GetGhost()->behavior->getPlayerToChase();
    if (player.hasValue()) {
        auto pos = player.player->GetNode()->GetWorldPosition();
        pos.y_ = GetGhost()->GetNode()->GetWorldPosition().y_;
        return GetGhost()->walkTo(pos);
    }
    return false;
}

void GhostHunt::restart() {
    GetGhost()->setNextState("Local", GetGhost()->behavior->huntDuration);
}
}
