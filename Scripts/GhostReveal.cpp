#include "GhostReveal.hpp"
#include "Ghost.hpp"
#include "Player.hpp"

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource3D.h>



namespace Game {
void GhostReveal::Initialize() {
    if (GetGhost()->getClosestPlayer().hasValue()) {
        revealMode = GetGhost()->behavior->getRevealMode(GetGhost()->getClosestPlayer().distance);
        if (revealMode != RevealMode::airball) {
            // Play reveal sound
            auto sound = GetNode()->GetOrCreateComponent<SoundSource3D>();
            sound->SetFarDistance(GetGhost()->behavior->vocalRange);
            sound->Play(GetSubsystem<ResourceCache>()->GetResource<Sound>("SFX/ghostSingMix.ogg"));
        }
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
        GetNode()->GetComponent<SoundSource3D>()->Play(GetSubsystem<ResourceCache>()->GetResource<Sound>("SFX/screamShort.wav"));
    }
}

void GhostReveal::FixedUpdate(float) {
    // RevealMode-dependent code
    switch (revealMode) {
    case RevealMode::chasing: {
        GetGhost()->walkTo(GetGhost()->getClosestPlayer().player->GetNode()->GetWorldPosition());
    } break;
    default: {}
    };
    // End reveal early if needed
    for (auto player : GetGhost()->levelManager->getPlayers()) {
        if (GetGhost()->getDistanceToPlayer(player) < 1.0f) {
            GetGhost()->setState("Local");
            break;
        }
    }
}
}
