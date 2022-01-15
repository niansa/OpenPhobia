namespace Game {
class Ghost;
}
#ifndef GHOST_HPP
#define GHOST_HPP
#include "../easyscript/Namespace.hpp"

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Math/RandomEngine.h>
#include <Urho3D/Physics/RigidBody.h>



namespace Game {
class Ghost final : public LogicComponent {
    URHO3D_OBJECT(Ghost, LogicComponent);

    RandomEngine rng;

public:
    using LogicComponent::LogicComponent;

    virtual void FixedUpdate(float timeStep) override;

    void throwBody(RigidBody *body);
};
}
#endif // GHOST_HPP
