#include "GhostReveal.hpp"
#include "Ghost.hpp"
#include "Player.hpp"
#include "Door.hpp"
#include "EMFEmitter.hpp"
#include "RoomBoundary.hpp"
#include "../SFX.hpp"

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
        // Get random player
        auto& players = GetGhost()->levelManager->getPlayers();
        auto player = players[GetGhost()->rng.GetUInt(0, players.size())];
        // Check that player is inside the house
        if (player->isInsideHouse()) {
            // TODO: Teleport ghost to some location around the player
            // Get reveal mode from behavior
            revealMode = GetGhost()->behavior->getRevealMode(player);
            // Play reveal sound (sometimes)
            if (GetGhost()->rng.GetBool(0.5f)) {
                auto sound = GetNode()->GetOrCreateComponent<SoundSource3D>();
                sound->SetFarDistance(GetGhost()->behavior->vocalRange);
                sound->Play(GetGhost()->levelManager->getGhostIdentity().revealSound);
            }
            // Walk towards player if in chasing mode
            if (revealMode == RevealMode::chasing) {
                GetGhost()->walkTo(closestPlayer.player->GetNode()->GetWorldPosition());
            }
            // Manifest
            GetGhost()->appearance->SetDeepEnabled(true);
            GetGhost()->setNextState("Local", GetGhost()->rng.GetUInt(2500, Max(15000.0f*GetGhost()->getAggression(), 1500)));
            // Close room doors
            if (GetGhost()->rng.GetBool(0.5f/*TBV*/)) {
                // Get all rooms doors  TODO: Better implementation
                for (auto body : GetGhost()->getCloseBodies()) {
                    if (body.body_) {
                        auto node = body.body_->GetNode();
                        if (node->GetName() == "Door") {
                            auto parentNode = node->GetParent();
                            if (parentNode && GetGhost()->getCurrentRoom()->isNodeInside(parentNode)) {
                                if (parentNode->HasComponent<Door>()) {
                                    auto door = parentNode->GetComponent<Door>();
                                    door->impulsePush(0.1f);
                                }
                            }
                        }
                    }
                }
            }
            // Spawn EMF Spot
            auto emitter = GetGhost()->spawnEMFSpot();
            emitter->setLevel(EMFLevel::reveal);
            emitter->timeoutIn(defaultEmfTimeout);
            return;
        }
    }
    aborted = true;
    GetGhost()->setState("Local");
}

void GhostReveal::Deinitialize() {
    if (!aborted) {
        if (revealMode == RevealMode::standing) {
            if (GetNode()->HasComponent<SoundSource3D>()) {
                // Stop vocal sound
                GetNode()->GetComponent<SoundSource3D>()->Stop();
            }
        } else {
            // Play hiss
            auto soundSource = GetNode()->GetOrCreateComponent<SoundSource3D>();
            soundSource->SetGain(0.75f);
            soundSource->Play(getGhostSFX(context_, GetGhost()->rng, "hiss", GetGhost()->getIdentity().gender));
        }
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
