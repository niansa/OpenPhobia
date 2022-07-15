#pragma once

namespace Game {
class GabroImport;
}
#ifndef GABROIMPORT_HPP
#define GABROIMPORT_HPP
#include "../easyscript/Namespace.hpp"
#include "Useable.hpp"

#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Physics/RigidBody.h>



namespace Game {
class GabroImport final : public LogicComponent {
    URHO3D_OBJECT(GabroImport, LogicComponent);

public:
    using LogicComponent::LogicComponent;

    void Start() override;
};
}
#endif // GABROIMPORT_HPP
