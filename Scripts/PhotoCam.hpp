#pragma once

namespace Game {
class PhotoCam;
}
#ifndef PHOTOCAM_HPP
#define PHOTOCAM_HPP
#include "../easyscript/Namespace.hpp"
#include "Useable.hpp"

#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Physics/RigidBody.h>



namespace Game {
class PhotoCam final : public Useable {
    URHO3D_OBJECT(PhotoCam, Useable);

public:
    using Useable::Useable;

    void TurnOn(void) override;
    void TurnOff(void) override;
};
}
#endif // PLAYER_HPP
