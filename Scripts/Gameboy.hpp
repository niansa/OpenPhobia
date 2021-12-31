namespace Game {
class Gameboy;
}
#ifndef GAMEBOY_HPP
#define GAMEBOY_HPP
#include "../easyscript/Namespace.hpp"

#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Physics/RigidBody.h>



namespace Game {
class Gameboy final : public Component {
    URHO3D_OBJECT(Gameboy, Component);

public:
    using Component::Component;
};
}
#endif // PLAYER_HPP
