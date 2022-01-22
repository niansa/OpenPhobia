namespace Game {
class Lightswitch;
}
#ifndef LIGHTSWITCH_HPP
#define LIGHTSWITCH_HPP
#include "../easyscript/Namespace.hpp"
#include "Useable.hpp"

#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Core/Timer.h>



namespace Game {
class Lightswitch final : public Useable {
    URHO3D_OBJECT(Lightswitch, Useable);

    eastl::vector<Light*> lightBulbs;
    Timer cooldown;

public:
    using Useable::Useable;

    virtual void Start() override;
    virtual void TurnOn() override;
    virtual void TurnOff() override;
};
}
#endif // PLAYER_HPP
