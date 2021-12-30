namespace Game {
class Jumper;
}
#ifndef JUMPER_HPP
#define JUMPER_HPP
#include "../easyscript/Namespace.hpp"
#include "../easyscript/Script.hpp"

#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Physics/RigidBody.h>



namespace Game {
class Jumper final : public Script {
public:
    virtual bool Update() override;
};
}
#endif // PLAYER_HPP
