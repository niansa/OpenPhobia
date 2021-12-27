namespace Scatter {
class Player;
}
#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "../Namespace.hpp"
#include "../Script.hpp"

#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Physics/RigidBody.h>



namespace Scatter {
class LevelManager;

class Player final : public Script {
    Node *head;
    LevelManager *lMan;

    void limitVel(Vector3& vel) {
        vel.x_ = Max(Min(vel.x_, 10), -10);
        vel.y_ = Max(Min(vel.y_, 10), -100);
        vel.z_ = Max(Min(vel.z_, 10), -10);
    }

public:
    void setLevelManager(LevelManager *lMan) {
        this->lMan = lMan;
    }

    virtual void Start() override {
        head = node->GetChild("Head");
    }

    virtual bool Update() override;

    Node *getHead() {
        return head;
    }
};
}
#endif // PLAYER_HPP
