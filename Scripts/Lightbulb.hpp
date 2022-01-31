#pragma once

namespace Game {
class Lightbulb;
}
#ifndef LIGHTBULB_HPP
#define LIGHTBULB_HPP
#include "../easyscript/Namespace.hpp"

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Graphics/Light.h>



namespace Game {
class Lightbulb final : public LogicComponent {
    URHO3D_OBJECT(Lightbulb, LogicComponent);

    Light *light;
    float originalBnes;
    bool fading = false;
    bool turnedOn;

public:
    using LogicComponent::LogicComponent;

    void Start() override;
    void FixedUpdate(float) override;

    Light *getLight() {
        return light;
    }
    bool isTurnedOn() {
        return turnedOn;
    }

    void TurnOn();
    void TurnOff();
};
}
#endif // PLAYER_HPP
