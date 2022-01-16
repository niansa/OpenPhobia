namespace Game {
class Ghost;
}
#ifndef GHOST_HPP
#define GHOST_HPP
#include "../easyscript/Namespace.hpp"

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Math/RandomEngine.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/KinematicCharacterController.h>



namespace Game {
enum class GhostState {
    local,
    roaming
};

class Ghost final : public LogicComponent {
    URHO3D_OBJECT(Ghost, LogicComponent);

    RandomEngine rng;
    Timer stepTimer, stateTimer;
    KinematicCharacterController* kinematicController;
    PhysicsWorld *physicsWorld;

public:
    GhostState state;

    using LogicComponent::LogicComponent;

    virtual void Start() override;
    virtual void FixedUpdate(float timeStep) override;

    void setState(GhostState nState);
    void throwBody(RigidBody *body);
    void useBody(RigidBody *body);
};
}
#endif // GHOST_HPP
