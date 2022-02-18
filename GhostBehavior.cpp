#include "GhostBehavior.hpp"
#include "Useable.hpp"
#include "GhostIdentity.hpp"
#include "Scripts/HouseBoundary.hpp"
#include "Scripts/Player.hpp"
#include "Scripts/Ghost.hpp"
#include "Scripts/GhostReveal.hpp"
#include "easyscript/Namespace.hpp"



namespace Game {
namespace GhostBehaviors {
float Default::getCurrentSpeed() {
    if (ghost->getState() == "Hunt") {
        auto playerToChase = getPlayerToChase();
        if (playerToChase.hasValue() && ghost->canSeePlayer(playerToChase)) {
            speedup = Min(speedup + 0.0015f, 3.5f);
        } else if (speedup != 0.0f) {
            speedup = Max(speedup - 0.005f, 0.0f);
        }
        return 1.0f + speedup;
    } else {
        return 1.0f;
    }
}

PlayerWDistance Default::getPlayerToChase() {
    ghost->updateClosestPlayer();
    return ghost->getClosestPlayer();
}

void Default::onStateChange(const eastl::string& nState) {
    if (nState == "Hunt") {
        speedup = 0.0f;
    }
}

float Default::getBlinkSpeed() {
    return ghost->rng.GetFloat(80.0f, 300.0f);
}

float Default::getThrowPower() {
    return ghost->rng.GetFloat(0.25f, 1.75f);
}

RevealMode Default::getRevealMode(Player *player) {
    // DNV
    auto distance = ghost->getDistanceToPlayer(player);
    if (distance < 2.0f) {
        return RevealMode::standing;
    } else if (distance < 10.0f) {
        return RevealMode::chasing;
    } else {
        return RevealMode::airball;
    }
}


void Wraith::FrequentUpdate(float) {

}


void Poltergeist::onInteraction(InteractionType::Type type) {
    // Do double-throw sometimes
    if (type == InteractionType::throw_ && ghost->rng.GetBool(0.2f/*TBV*/)) {
        ghost->tryInteract(InteractionType::throw_);
    }
}


float Phantom::getBlinkSpeed() {
    return ghost->rng.GetFloat(1000.0f, 2000.0f);
}


float Jinn::getCurrentSpeed() {
    if (ghost->getState() == "Hunt") {
        // Add speed if the ghost is able to see the player
        auto player = getPlayerToChase();
        if (ghost->canSeePlayer(player) && player.distance > 2.0f) {
            return 2.5f;
        }
    }
    return Default::getCurrentSpeed();
}


float Revenant::getCurrentSpeed() {
    if (ghost->getState() == "Hunt") {
        // Add speed if the ghost is able to see the player
        if (ghost->canSeePlayer(getPlayerToChase())) {
            return 2.0f;
        } else {
            return 0.5f;
        }
    }
    return 1.0f;
}


float Shade::getAgressionMultiplier() {
    ghost->updateClosestPlayer();
    if (ghost->getClosestPlayer().distance < 7.5f/*TBV*/) {
        return 0.5f;
    } else {
        return 1.0f;
    }
}


float Oni::getThrowPower() {
    return ghost->rng.GetFloat(0.75f, 2.0f);
}

float Oni::getAgressionMultiplier() {
    ghost->updateClosestPlayer();
    if (ghost->getClosestPlayer().distance < 7.5f/*TBV*/) {
        return 1.5f;
    } else {
        return 1.0f;
    }
}


float Raiju::getCurrentSpeed() {
    auto bSpeed = Default::getCurrentSpeed();
    if (ghost->getState() == "Hunt") {
        // Check if there is any turned on electronic turned on nearby
        for (auto body : ghost->getCloseBodies()) {
            if (body.body_ && body.distance_ <= 3.0f) {
                auto node = body.body_->GetNode();
                if (ghost->levelManager->getHouse()->isNodeInside(node) && node->HasTag("Useable")) {
                    auto script = static_cast<Useable *>(node->GetComponent(node->GetName()));
                    if (script->isTurnedOn()) {
                        // Make it faster
                        bSpeed += 1.5f;
                        break;
                    }
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
    return fres;
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
