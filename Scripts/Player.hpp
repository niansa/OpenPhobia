#pragma once

namespace Game {
class Player;
}
#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "../easyscript/Namespace.hpp"

#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/KinematicCharacterController.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>



namespace Game {
class RoomBoundary;
class LevelManager;
class Door;

class Player final : public LogicComponent {
    URHO3D_OBJECT(Player, LogicComponent);

    static constexpr float walkSpeed = 1.6f,
                           runSpeed = 3.0f,
                           stamina = 5000.0f,
                           grabRange = 2.f,
                           interactRange = 4.0f,
                           throwDistance = 0.5f;

    LevelManager *levelManager;
    Node *head;
    Node *handP;
    Node *hand = nullptr;
    Door *lastDoor = nullptr;

    CollisionShape* collisionShape;
    KinematicCharacterController* kinematicController;

public:
    unsigned sanity = 25;

    using LogicComponent::LogicComponent;

    void Start() override;
    void FixedUpdate(float timeStep) override;
    void Update(float timeStep) override;

    Node *getHand() {
        return hand;
    }
    Node *getHead() {
        return head;
    }

    void grab(Node *node);
    void drop();
    void startKillingPlayer() {}
    RoomBoundary *getCurrentRoom();
    bool isInsideHouse();
};
}
#endif // PLAYER_HPP
