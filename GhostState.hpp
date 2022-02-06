#pragma once

namespace Game {
class GhostHunt;
}
#ifndef GHOSTSTATE_HPP
#define GHOSTSTATE_HPP
#include "easyscript/Namespace.hpp"

#include <Urho3D/Scene/LogicComponent.h>



namespace Game {
class Ghost;

class GhostStateScript : public LogicComponent {
    friend Ghost;
    Ghost *ghost;

public:
    using LogicComponent::LogicComponent;

    virtual void Initialize() = 0;
    virtual void Deinitialize() {};

    Ghost *GetGhost() {
        return ghost;
    }
};
}
#endif // GHOSTSTATE_HPP
