#pragma once

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
        } else {
            TurnOn();
        }
    }
    void GhostUse(bool respectCooldown = true) {
        if (!respectCooldown || ghostUseTimer.GetMSec(false) > ghostUseCooldown) {
            if (turnedOn) {
                GhostTurnOff();
            } else {
                GhostTurnOn();
            }
            ghostUseTimer.Reset();
        }
    }
    virtual void TurnOn() {turnedOn = true;}
    virtual void TurnOff() {turnedOn = false;}
    virtual void GhostTurnOn() {TurnOn();}
    virtual void GhostTurnOff() {TurnOff();}

    bool isTurnedOn() {
        return turnedOn;
    }
};
}
#endif
