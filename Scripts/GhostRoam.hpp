#pragma once

namespace Game {
class GhostRoam;
}
#ifndef GHOSTROAM_HPP
#define GHOSTROAM_HPP
#include "../easyscript/Namespace.hpp"
#include "../GhostState.hpp"



namespace Game {
class Ghost;

class GhostRoam final : public GhostStateScript {
    URHO3D_OBJECT(GhostRoam, GhostStateScript);

public:
    using GhostStateScript::GhostStateScript;

    void Initialize() override;
    void Deinitialize() override;
    void FixedUpdate(float) override;
};
}
#endif // GHOST_HPP
