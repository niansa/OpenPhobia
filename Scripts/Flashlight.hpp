#pragma once

namespace Game {
class Flashlight;
}
#ifndef FLASHLIGHT_HPP
#define FLASHLIGHT_HPP
#include "../easyscript/Namespace.hpp"
#include "Equipment.hpp"

#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/Physics/RigidBody.h>



namespace Game {
class Flashlight final : public Equipment {
    URHO3D_OBJECT(Flashlight, Equipment);

    Timer blinkTimer;
    float nextBlinkIn = 0.0f;

public:
    using Equipment::Equipment;

    void Update(float) override;
    void Start() override;
    void TurnOn() override;
    void TurnOff() override;

    void playClick();
};
}
#endif // PLAYER_HPP
