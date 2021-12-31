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
class LevelManager;

class Player final : public Script {
    Node *head;
    LevelManager *lMan;

    void limitVel(Vector3& vel) {
        vel.x_ = Max(Min(vel.x_, 10), -10);
        vel.y_ = Max(Min(vel.y_, 10), -100);
        vel.z_ = Max(Min(vel.z_, 10), -10);
    }

protected:
    CollisionShape* collisionShape;
    KinematicCharacterController* kinematicController;

public:
    using Script::Script;

    void setLevelManager(LevelManager *lMan) {
        this->lMan = lMan;
    }

    virtual void Start() override;
    virtual bool Update() override;

    Node *getHead() {
        return head;
    }

    void onCollision(StringHash eventType, VariantMap& eventData);
};
}
#endif // PLAYER_HPP
