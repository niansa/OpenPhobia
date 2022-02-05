#include "GhostBehavior.hpp"
#include "Useable.hpp"
#include "Scripts/Player.hpp"
#include "Scripts/Ghost.hpp"
#include "easyscript/Namespace.hpp"



namespace Game {
namespace GhostBehaviors {
float Default::getCurrentSpeed() {
    auto playerToChase = getPlayerToChase();
    if (ghost->getState() == GhostState::hunt && playerToChase.hasValue() && ghost->canSeePlayer(playerToChase)) {
        speedup = Min(speedup + 0.0015f, 3.5f);
    } else if (speedup != 0.0f) {
        speedup = Max(speedup - 0.005f, 0.0f);
    }
    return 1.0f + speedup;
}

PlayerWDistance Default::getPlayerToChase() {
    return ghost->getClosestPlayer();
}

void Default::onHuntStart() {
    speedup = 0.0f;
}

float Default::getBlinkSpeed() {
    return ghost->rng.GetFloat(80.0f, 300.0f);
}


float Phantom::getBlinkSpeed() {
    return ghost->rng.GetFloat(1000.0f, 2000.0f);
}


float Revenant::getCurrentSpeed() {
    auto bSpeed = Default::getCurrentSpeed();
    // Add speed if the ghost is able to see the player
    if (ghost->canSeePlayer(getPlayerToChase())) {
        bSpeed += 2.0f;
    } else {
        bSpeed -= 0.5f;
    }
    // Return final speed
    return bSpeed;
}


float Raiju::getCurrentSpeed() {
    auto bSpeed = Default::getCurrentSpeed();
    // Check if there is any turned on electronic turned on nearby
    for (auto body : ghost->getCloseBodies()) {
        if (body.body_ && body.distance_ <= 3.0f) {
            auto node = body.body_->GetNode();
            if (node->HasTag("Useable")) {
                auto script = static_cast<Useable *>(node->GetComponent(node->GetName()));
                if (script->isTurnedOn()) {
                    // Make it faster
                    bSpeed += 1.5f;
                    break;
                }
            }
        }
    }
    // Return final speed
    return bSpeed;
}

bool Obake::hasEvidence(Evidence::Type checkedFor) {
    auto fres = Default::hasEvidence(checkedFor);
    if (checkedFor == Evidence::Fingerprints && ghost->rng.GetBool(0.25f)) {
        fres = false;
    }
    return true;
}
}

eastl::unique_ptr<GhostBehavior> getGhostBehavior(GhostType t) {
#   define handlecase(name) case GhostType::name: return eastl::make_unique<GhostBehaviors::name>()
    switch (t) {
    handlecase(Spirit);
    handlecase(Wraith);
    handlecase(Phantom);
    handlecase(Poltergeist);
    handlecase(Banshee);
    handlecase(Jinn);
    handlecase(Mare);
    handlecase(Revenant);
    handlecase(Shade);
    handlecase(Demon);
    handlecase(Yurei);
    handlecase(Oni);
    handlecase(Yokai);
    handlecase(Hantu);
    handlecase(Goryo);
    handlecase(Myling);
    handlecase(Onryo);
    handlecase(Twins);
    handlecase(Raiju);
    handlecase(Obake);
    handlecase(Mimic);
    default: return nullptr;
    }
}
}
