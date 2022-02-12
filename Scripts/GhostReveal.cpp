#include "GhostReveal.hpp"
#include "Ghost.hpp"
#include "Player.hpp"

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource3D.h>



namespace Game {
void GhostReveal::Initialize() {
    // Update and get closest player
    GetGhost()->updateClosestPlayer();
    auto closestPlayer = GetGhost()->getClosestPlayer();
    // Check that the ghost can see the player (and the player the ghost :P)
    if (closestPlayer.hasValue() && GetGhost()->canSeePlayer(closestPlayer)) {
        // Get reveal mode from behavior
        revealMode = GetGhost()->behavior->getRevealMode(GetGhost()->getClosestPlayer().distance);
        // Play reveal sound (sometimes)
        if (GetGhost()->rng.GetBool(0.5f)) {
            auto sound = GetNode()->GetOrCreateComponent<SoundSource3D>();
            sound->SetFarDistance(GetGhost()->behavior->vocalRange);
            sound->Play(GetSubsystem<ResourceCache>()->GetResource<Sound>("SFX/ghostSingMix.ogg"));
        }
        // Walk towards player if in chasing mode
        if (revealMode == RevealMode::chasing) {
            GetGhost()->walkTo(closestPlayer.player->GetNode()->GetWorldPosition());
        }
        // Manifest
        GetGhost()->appearance->SetDeepEnabled(true);
        GetGhost()->setNextState("Local", GetGhost()->rng.GetUInt(2500, Max(15000.0f*GetGhost()->getAggression(), 1500)));
    } else {
        GetGhost()->setState("Local");
    }
}

void GhostReveal::Deinitialize() {
    if (revealMode == RevealMode::standing) {
        // Stop vocal sound
        GetNode()->GetComponent<SoundSource3D>()->Stop();
    } else {
        // Play scream
        GetNode()->GetOrCreateComponent<SoundSource3D>()->Play(GetSubsystem<ResourceCache>()->GetResource<Sound>("SFX/screamShort.wav"));
    }
}

void GhostReveal::FixedUpdate(float) {
    // End reveal early if needed
    if (revealMode == RevealMode::chasing && GetGhost()->currentPath.empty()) {
        GetGhost()->setState("Local");
        return;
    }
    for (auto player : GetGhost()->levelManager->getPlayers()) {
        if (GetGhost()->getDistanceToPlayer(player) < 1.0f) {
            GetGhost()->setState("Local");
            break;
        }
    }
}
}
