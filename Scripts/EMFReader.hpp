namespace Game {
class EMFReader;
}
#ifndef EMFREADER_HPP
#define EMFREADER_HPP
#include "../easyscript/Namespace.hpp"
#include "Useable.hpp"

#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Physics/RigidBody.h>



namespace Game {
class EMFReader final : public Useable {
    URHO3D_OBJECT(EMFReader, Useable);

public:
    using Useable::Useable;

    virtual void TurnOn() override;
};
}
#endif // EMFREADER_HPP
