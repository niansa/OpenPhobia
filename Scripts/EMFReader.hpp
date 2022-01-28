#pragma once

namespace Game {
class EMFReader;
}
#ifndef EMFREADER_HPP
#define EMFREADER_HPP
#include "../easyscript/Namespace.hpp"
#include "Equipment.hpp"

#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Scene/LogicComponent.h>



namespace Game {
class EMFReader final : public Equipment {
    URHO3D_OBJECT(EMFReader, Equipment);

    Node *leds;

public:
    using Equipment::Equipment;

    virtual void Start() override;
    virtual void FixedUpdate(float) override;
    virtual void TurnOn() override;
    virtual void TurnOff() override;

    void setLevel(uint8_t level);
};
}
#endif // EMFREADER_HPP
