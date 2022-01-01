namespace Game {
class Useable;
}
#ifndef USEABLE_HPP
#define USEABLE_HPP
#include "easyscript/Namespace.hpp"

#include <Urho3D/Scene/LogicComponent.h>



namespace Game {
class Useable : public LogicComponent {
protected:
    bool turnedOn = false;

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
    virtual void TurnOn() {}
    virtual void TurnOff() {}
};
}
#endif
