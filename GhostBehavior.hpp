#pragma once

namespace Game {
struct PlayerWDistance;
struct GhostBehavior;
}
#ifndef GHOSTBEHAVIOR_HPP
#define GHOSTBEHAVIOR_HPP
#include "easyscript/Namespace.hpp"



namespace Game {
class Ghost;
class Player;

struct PlayerWDistance {
    Player *player = nullptr;
    float distance;

    bool hasValue() {
        return player;
    }
};

struct GhostBehavior {
    Ghost *ghost;
    unsigned huntDuration = 30;
    unsigned sanityThreshold = 50;
    unsigned huntCooldown = 30;
    unsigned gracePeriod = 3;
    unsigned smudgeEffectDuration = 90;
    float speedup = 0.0f;
    float agression = 1.0f;
    bool superHardThrows = false;

    virtual ~GhostBehavior() {}
    virtual float getCurrentSpeed() = 0;
    virtual PlayerWDistance getPlayerToChase() = 0;
    virtual unsigned getHuntMultiplier() = 0;
    virtual void onHuntStart() = 0;
    virtual float getFlashlightBlinkSpeed() = 0;
};
namespace GhostBehaviors {
struct Default : public GhostBehavior {
    Default() {}
    float getCurrentSpeed() override;
    PlayerWDistance getPlayerToChase() override;
    unsigned getHuntMultiplier() override {return 0;}
    void onHuntStart() override;
    float getFlashlightBlinkSpeed() override;
};

struct Shade : public Default {
    Shade() {
        sanityThreshold = 35;
        agression = 0.25f;
    }
};
struct Spirit : public Default {
    Spirit() {
        smudgeEffectDuration = 180;
    }
};
struct Poltergeist : public Default {
    Poltergeist() {
        superHardThrows = true;
    }
};
struct Phantom : public Default {
    Phantom() {}

    float getFlashlightBlinkSpeed() override;
};
struct Oni : public Default {
    Oni() {
        superHardThrows = true;
        agression = 1.75f;
    }
};
struct Raiju : public Default {
    Raiju() {
        sanityThreshold = 65;
    }

    float getCurrentSpeed() override;
};
struct Demon : public Default {
    Demon() {
        sanityThreshold = 70;
        agression = 1.25f;
        huntCooldown = 20;
        smudgeEffectDuration = 60;
    }
};
struct Yokai : public Default {
    Yokai() {
        sanityThreshold = 80;
    }
};
struct Revenant : public Default {
    Revenant() {}

    float getCurrentSpeed() override;
};
}
}
#endif // GHOSTBEHAVIOR_HPP
