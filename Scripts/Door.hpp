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

    static constexpr float maxAngle = 90.0f,
                           moveSpeed = 2.0;

    RigidBody *doorBody;
    Node *doorNode;
    enum State {opening, closing, still} state = still;
    bool forced = false;

    void setState(State nState);

public:
    using LogicComponent::LogicComponent;

    void Start() override;
    void FixedUpdate(float) override;

    void close(bool force = false) {
        forced = force;
        setState(closing);
    }
    void open(bool force = false) {
        forced = force;
        setState(opening);
    }

    void Activate();
};
}
#endif // DOOR_HPP
