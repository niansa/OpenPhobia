#pragma once

namespace Game {
class Gameboy;
}
#ifndef EMFEMITTER_HPP
#define EMFEMITTER_HPP
#include "../easyscript/Namespace.hpp"
#include "../EMF.hpp"

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Core/Timer.h>



namespace Game {
class EMFEmitter final : public LogicComponent {
    URHO3D_OBJECT(EMFEmitter, LogicComponent);

    EMFLevel level = EMFLevel::basic;
    Timer timer;
    float timeout = 0;

public:
    using LogicComponent::LogicComponent;

    virtual void FixedUpdate(float) override;

    unsigned getLevel() {
        return static_cast<unsigned>(level);
    }
    void setLevel(EMFLevel level) {
        this->level = level;
    }
    void timeoutIn(float seconds) {
        timeout = seconds * 1000;
        timer.Reset();
    }
};
}
#endif // EMFEMITTER_HPP
