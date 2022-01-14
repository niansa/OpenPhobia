namespace Game {
class Gameboy;
}
#ifndef EMFEMITTER_HPP
#define EMFEMITTER_HPP
#include "../easyscript/Namespace.hpp"
#include "../EMF.hpp"

#include <Urho3D/Scene/Component.h>



namespace Game {
class EMFEmitter final : public Component {
    URHO3D_OBJECT(EMFEmitter, Component);

    EMFLevel level = EMFLevel::basic;

public:
    using Component::Component;

    unsigned getLevel() {
        return static_cast<unsigned>(level);
    }
    void setLevel(EMFLevel level) {
        this->level = level;
    }
};
}
#endif // EMFEMITTER_HPP
