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
#include <Urho3D/Physics/RigidBody.h>



namespace Game {
class Flashlight final : public Equipment {
    URHO3D_OBJECT(Flashlight, Equipment);

public:
    using Equipment::Equipment;

    virtual void Start() override;
    virtual void TurnOn() override;
    virtual void TurnOff() override;
};
}
#endif // PLAYER_HPP
