#pragma once

namespace Game {
class Door;
}
#ifndef DOOR_HPP
#define DOOR_HPP
#include "../easyscript/Namespace.hpp"

#include <Urho3D/Scene/LogicComponent.h>



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

    Vector3 currentRotation;
    float maxAngle;
    float baseAngle;
    float impulse = 0.0f;

public:
    float impulseSpeed = 0.25f;

    using LogicComponent::LogicComponent;

    void Start() override;
    void FixedUpdate(float) override;

    float getClosedAngle() const {
        return baseAngle;
    }
    float getFullyOpenAngle() const {
        return baseAngle+maxAngle;
    }
    float getRelativeFullyOpenAngle() const {
        return maxAngle;
    }
    float getAngle() const {
        return currentRotation.y_; //TODO: Inline
    }
    bool isClosed() const {
        return getRelativeAngle() < 1.0f;
    }
    bool isFullyOpen() const {
        return getRelativeAngle() >= getRelativeFullyOpenAngle() - 1.0f;
    }

    float getRelativeAngle() const;
    bool getSmartDir() const;
    void push(float power);
    void push(float power, bool direction);
    void impulsePush(float power);
    void impulsePush(float power, bool direction);
};
}
#endif // DOOR_HPP
