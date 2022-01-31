#pragma once

namespace Game {
class Gameboy;
}
#ifndef GAMEBOY_HPP
#define GAMEBOY_HPP
#include "../easyscript/Namespace.hpp"
#include "Useable.hpp"

#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Physics/RigidBody.h>



namespace Game {
class Gameboy final : public Useable {
    URHO3D_OBJECT(Gameboy, Useable);

public:
    using Useable::Useable;

    void TurnOn() override;
    void TurnOff() override;
};
}
#endif // PLAYER_HPP
