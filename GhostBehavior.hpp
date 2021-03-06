#pragma once

namespace Game {
struct PlayerWDistance;
struct GhostBehavior;
}
#ifndef GHOSTBEHAVIOR_HPP
#define GHOSTBEHAVIOR_HPP
#include "easyscript/Namespace.hpp"
#include "Evidence.hpp"

#include <EASTL/string.h>
#include <EASTL/map.h>
#include <EASTL/unique_ptr.h>



namespace Game {
class Ghost;
class Player;
enum class EMFLevel;
enum class RevealMode;
enum class GhostType;

namespace InteractionType {
using Type = uint8_t;
enum : Type {
    touch = 0b10,
    throw_ = 0b01,
    any = 0b11,
    none = 0b00
};
}

struct PlayerWDistance {
    Player *player = nullptr;
    float distance;

    bool hasValue() {
        return player;
    }
};

struct GhostBehavior {
    eastl::string name;
    Ghost *ghost;
    unsigned huntDuration = 30000;
    unsigned sanityThreshold = 50;
    unsigned huntCooldown = 25000;
    unsigned interactionCooldown = 2000;
    unsigned gracePeriod = 3000;
    unsigned smudgeEffectDuration = 90000;
    float electricVisionRange = 10.0f;
    float hearingRange = 10.0f;
    float electricDistortionRange = 10.0f;
    float vocalRange = 20.0f;
    float footstepRange = 20.0f;
    float speedup = 0.0f;
    float agression = 1.0f;
    bool endHuntOnDeath = true;
    Evidence::Type evidence = 0;

    virtual ~GhostBehavior() {}
    virtual void FrequentUpdate(float) = 0;
    virtual float getCurrentSpeed() = 0;
    virtual PlayerWDistance getPlayerToChase() = 0;
    virtual unsigned getHuntMultiplier() = 0;
    virtual float getAgressionMultiplier() = 0;
    virtual EMFLevel getEMFLevel(EMFLevel level) = 0;
    virtual void onStateChange(const eastl::string& nState) = 0;
    virtual void onInteraction(InteractionType::Type type) = 0;
    virtual float getBlinkSpeed() = 0;
    virtual float getThrowPower() = 0;
    virtual bool hasEvidence(Evidence::Type) = 0;
    virtual RevealMode getRevealMode(Player *player) = 0;
};

namespace GhostBehaviors {
struct Default : public GhostBehavior {
    Default() {}
    void FrequentUpdate(float) override {}
    float getCurrentSpeed() override;
    PlayerWDistance getPlayerToChase() override;
    unsigned getHuntMultiplier() override {return 0;}
    float getAgressionMultiplier() override {return 1.0f;}
    EMFLevel getEMFLevel(EMFLevel expected) override;
    void onStateChange(const eastl::string& nState) override;
    void onInteraction(InteractionType::Type type) override {};
    float getBlinkSpeed() override;
    float getThrowPower() override;
    bool hasEvidence(Evidence::Type checkedFor) override {
        return Evidence::hasEvidence(evidence, checkedFor);
    }
    RevealMode getRevealMode(Player *player) override;
};
struct Spirit : public Default {
    Spirit() {
        name = "Spirit";
        using namespace Evidence;
        evidence = EMFLevelFive | SpiritBox | GhostWriting;
        smudgeEffectDuration = 180;
    }
};
struct Wraith : public Default {
    Wraith() {
        name = "Wraith";
        using namespace Evidence;
        evidence = EMFLevelFive | SpiritBox | DOTSProjection;
    }

    void FrequentUpdate(float) override;
};
struct Phantom : public Default {
    Phantom() {
        name = "Phantom";
        using namespace Evidence;
        evidence = SpiritBox | Fingerprints | DOTSProjection;
    }

    float getBlinkSpeed() override;
};
struct Poltergeist : public Default {
    Poltergeist() {
        name = "Poltergeist";
        using namespace Evidence;
        evidence = SpiritBox | Fingerprints | GhostWriting;
    }

    void onInteraction(InteractionType::Type type) override;
};
struct Banshee : public Default {
    Banshee() {
        name = "Banshee";
        using namespace Evidence;
        evidence = SpiritBox | GhostOrbs | Fingerprints;
    }
};
struct Jinn : public Default {
    Jinn() {
        name = "Jinn";
        using namespace Evidence;
        evidence = EMFLevelFive | Fingerprints | FreezingTemps;
    }

    float getCurrentSpeed() override;
};
struct Mare : public Default {
    Mare() {
        name = "Mare";
        using namespace Evidence;
        evidence = SpiritBox | GhostOrbs | GhostWriting;
    }

    void onStateChange(const eastl::string& nState) override;
    unsigned getHuntMultiplier() override;
};
struct Revenant : public Default {
    Revenant() {
        name = "Revenant";
        using namespace Evidence;
        evidence = GhostOrbs | GhostWriting | FreezingTemps;
    }

    float getCurrentSpeed() override;
};
struct Shade : public Default {
    Shade() {
        name = "Shade";
        using namespace Evidence;
        evidence = EMFLevelFive | GhostWriting | FreezingTemps;
        sanityThreshold = 35;
        agression = 0.25f;
    }

    float getAgressionMultiplier() override;
};
struct Demon : public Default {
    Demon() {
        name = "Demon";
        using namespace Evidence;
        evidence = Fingerprints | GhostWriting | FreezingTemps;
        sanityThreshold = 70;
        agression = 1.25f;
        huntCooldown = 20;
        smudgeEffectDuration = 60;
    }
};
struct Yurei : public Default {
    Yurei() {
        name = "Yurei";
        using namespace Evidence;
        evidence = GhostOrbs | FreezingTemps | DOTSProjection;
    }
};
struct Oni : public Default {
    Oni() {
        name = "Oni";
        using namespace Evidence;
        evidence = EMFLevelFive | FreezingTemps | DOTSProjection;
        agression = 1.75f;
    }

    float getThrowPower() override;
    float getAgressionMultiplier() override;
};
struct Yokai : public Default {
    Yokai() {
        name = "Yokai";
        using namespace Evidence;
        evidence = SpiritBox | GhostOrbs | DOTSProjection;
        sanityThreshold = 80; // <- TODO: Use sanity modifier instead
        hearingRange = 2.0f;
    }
};
struct Hantu : public Default {
    Hantu() {
        name = "Hantu";
        using namespace Evidence;
        evidence = Fingerprints | GhostOrbs | FreezingTemps;
    }
};
struct Goryo : public Default {
    Goryo() {
        name = "Goryo";
        using namespace Evidence;
        evidence = EMFLevelFive | Fingerprints | DOTSProjection;
    }
};
struct Myling : public Default {
    Myling() {
        name = "Myling";
        using namespace Evidence;
        evidence = EMFLevelFive | Fingerprints | GhostWriting;
        footstepRange = 10.0f;
    }
};
struct Onryo : public Default {
    Onryo() {
        name = "Onryo";
        using namespace Evidence;
        evidence = SpiritBox | GhostOrbs | FreezingTemps;
    }
};
struct Twins : public Default {
    Twins() {
        name = "The Twins";
        using namespace Evidence;
        evidence = EMFLevelFive | SpiritBox | FreezingTemps;
    }
};
struct Raiju : public Default {
    Raiju() {
        name = "Raiju";
        using namespace Evidence;
        evidence = EMFLevelFive | GhostOrbs | DOTSProjection;
        sanityThreshold = 65; // <- TODO: Use sanity modifier instead
    }

    float getCurrentSpeed() override;
};
struct Obake : public Default {
    Obake() {
        name = "Obake";
        using namespace Evidence;
        evidence = EMFLevelFive | Fingerprints | GhostOrbs;
    }

    bool hasEvidence(Evidence::Type checkedFor) override;
};
struct Mimic : public Default {
    Mimic() {
        name = "The Mimic";
        using namespace Evidence;
        evidence = SpiritBox | Fingerprints | FreezingTemps | GhostOrbs;
    }
};
}

eastl::unique_ptr<GhostBehavior> getGhostBehavior(GhostType);
}
#endif // GHOSTBEHAVIOR_HPP
