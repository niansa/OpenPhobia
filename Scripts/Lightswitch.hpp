#pragma once

namespace Game {
class Lightswitch;
}
#ifndef LIGHTSWITCH_HPP
#define LIGHTSWITCH_HPP
#include "../easyscript/Namespace.hpp"
#include "Useable.hpp"
#include "Lightbulb.hpp"

#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Core/Timer.h>



namespace Game {
class Lightswitch final : public Useable {
    URHO3D_OBJECT(Lightswitch, Useable);

    eastl::vector<Lightbulb*> lightBulbs;
    Timer cooldown;
    bool ghostyDimmed = false;

public:
    using Useable::Useable;

    void Start() override;
    void TurnOn() override;
    void TurnOff() override;

    void ghostyDim(bool enable);
};
}
#endif // PLAYER_HPP
