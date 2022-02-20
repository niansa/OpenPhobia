#pragma once

namespace Game {
class GhostHunt;
}
#ifndef GHOSTHUNT_HPP
#define GHOSTHUNT_HPP
#include "../easyscript/Namespace.hpp"
#include "../GhostState.hpp"

#include <Urho3D/Core/Timer.h>



namespace Game {
class Ghost;

class GhostHunt final : public GhostStateScript {
    URHO3D_OBJECT(GhostHunt, GhostStateScript);

    Timer gracePeriodTimer, blinkTimer, stepTimer;
    unsigned nextBlinkIn = 0;
    bool aborted = false;

public:
    using GhostStateScript::GhostStateScript;

    void Initialize() override;
    void Deinitialize() override;
    void FixedUpdate(float) override;

    bool chasePlayer();
    void restart();
};
}
#endif // GHOST_HPP
