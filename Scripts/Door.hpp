#pragma once

namespace Game {
class Door;
}
#ifndef DOOR_HPP
#define DOOR_HPP
#include "../easyscript/Namespace.hpp"

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Physics/RigidBody.h>



namespace Game {
class Door final : public LogicComponent {
    URHO3D_OBJECT(Door, LogicComponent);

    float power;

    RigidBody *doorBody;
    Node *doorNode;

    bool moving = true;

public:
    using LogicComponent::LogicComponent;

    void Start() override;

    void push(float power = 1.0f);
};
}
#endif // DOOR_HPP
