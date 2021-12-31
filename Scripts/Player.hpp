namespace Game {
class Player;
}
#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "../easyscript/Namespace.hpp"
#include "../easyscript/Script.hpp"

#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/KinematicCharacterController.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>



namespace Game {
class Player final : public LogicComponent {
    URHO3D_OBJECT(Player, LogicComponent);

    Node *head;

protected:
    CollisionShape* collisionShape;
    KinematicCharacterController* kinematicController;

public:
    using LogicComponent::LogicComponent;

    virtual void Start() override;
    virtual void FixedUpdate(float timeStep) override;
};
}
#endif // PLAYER_HPP
