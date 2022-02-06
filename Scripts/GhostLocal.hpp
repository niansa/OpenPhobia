#pragma once

namespace Game {
class GhostLocal;
}
#ifndef GHOSTLOCAL_HPP
#define GHOSTLOCAL_HPP
#include "../easyscript/Namespace.hpp"
#include "../GhostState.hpp"



namespace Game {
class Ghost;

class GhostLocal final : public GhostStateScript {
    URHO3D_OBJECT(GhostLocal, GhostStateScript);

public:
    using GhostStateScript::GhostStateScript;

    void Initialize() override;
    void FixedUpdate(float) override;
};
}
#endif // GHOST_HPP
