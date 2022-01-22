namespace Game {
class Useable;
}
#ifndef USEABLE_HPP
#define USEABLE_HPP
#include "easyscript/Namespace.hpp"

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Core/Timer.h>



namespace Game {
class Useable : public LogicComponent {
protected:
    bool turnedOn = false;
    time_t ghostUseCooldown = 5000;
    Timer ghostUseTimer;

public:
    using LogicComponent::LogicComponent;

    virtual void Use() {
        if (turnedOn) {
            TurnOff();
            turnedOn = false;
        } else {
            TurnOn();
            turnedOn = true;
        }
    }
    void GhostUse(bool respectCooldown = true) {
        if (!respectCooldown || ghostUseTimer.GetMSec(false) > ghostUseCooldown) {
            Use();
            ghostUseTimer.Reset();
        }
    }
    virtual void TurnOn() {}
    virtual void TurnOff() {}
};
}
#endif
