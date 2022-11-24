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

    /************************
     * This entire class is *
     *                      *
     * DDDD  N    N V     V *
     * D   D NN   N V     V *
     * D   D N N  N V     V *
     * D   D N N  N  V   V  *
     * D   D N  N N  V   V  *
     * D   D N  N N   V V   *
     * D   D N   NN   V V   *
     * DDDD  N    N    V    *
     ***********************/

    RigidBody *doorBody;
    Node *doorNode;
    float minAngle, maxAngle;
    bool negDir;

public:
    using LogicComponent::LogicComponent;

    void Start() override;
    void FixedUpdate(float) override;

    float getClosedAngle() const {
        return negDir?maxAngle:minAngle;
    }
    float getOpenAngle() const {
        return negDir?minAngle:maxAngle;
    }
    bool isClosed() const {
        return getRelativeAngle() < 2.5f;
    }

    float getAngle() const;
    float getRelativeAngle() const;
    float getRelativeOpenAngle() const;
    bool getSmartDir() const;
    void push(float power, bool direction = false);
    void impulsePush(float power, bool direction = false);
};
}
#endif // DOOR_HPP
