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
    AutoRemoveMode removeMode = AutoRemoveMode::REMOVE_COMPONENT;

public:
    using LogicComponent::LogicComponent;

    void FixedUpdate(float) override;

    unsigned getLevel() {
        return static_cast<unsigned>(level);
    }
    void setLevel(EMFLevel level) {
        this->level = level;
    }
    void setAutoRemoveMode(AutoRemoveMode mode) {
        removeMode = mode;
    }
    void timeoutIn(float ms) {
        timeout = ms;
        timer.Reset();
    }

    static EMFEmitter *spawnSpot(Scene *scene, const Vector3& position);
};
}
#endif // EMFEMITTER_HPP
