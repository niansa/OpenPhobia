#pragma once

namespace Game {
class GhostReveal;
}
#ifndef GHOSTREVEAL_HPP
#define GHOSTREVEAL_HPP
#include "../easyscript/Namespace.hpp"
#include "../GhostState.hpp"



namespace Game {
class Ghost;

enum class RevealMode {
    standing,
    chasing,
    airball
};

class GhostReveal final : public GhostStateScript {
    URHO3D_OBJECT(GhostReveal, GhostStateScript);

    RevealMode revealMode;

public:
    using GhostStateScript::GhostStateScript;

    void Initialize() override;
    void Deinitialize() override;
    void FixedUpdate(float) override;
};
}
#endif // GHOST_HPP
