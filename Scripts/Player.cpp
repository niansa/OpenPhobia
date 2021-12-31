#include "Player.hpp"
#include "../LevelManager.hpp"

#include <Urho3D/Physics/CollisionShape.h>



namespace Game {
void Player::Start() {
    head = node->GetChild("Head");
    collisionShape = node->GetComponent<CollisionShape>();
    kinematicController = node->CreateComponent<KinematicCharacterController>();
    kinematicController->SetHeight(0.85f);
}

bool Player::Update() {
    auto* input = app->GetSubsystem<Input>();

    // Escape key handling
    if (input->GetKeyDown(Key::KEY_ESCAPE)) {
        exit(0);
    }

    // Position check
    if (node->GetPosition().y_ < -10) {
        lMan->reloadLevel();
        return false;
    }

    // Camera
    auto mMove = input->GetMouseMove();
    if (mMove.x_ || mMove.y_) {
        { // Head rotation
            auto headRot = head->GetRotation().EulerAngles();
            headRot.x_ = Max(Min(headRot.x_ + mMove.y_ / 8.0f, 80), -80);
            head->SetRotation(Quaternion(headRot));
        }
        { // Body rotation
            node->Rotate({0, mMove.x_ / 4.0f, 0});
        }
    }

    // Kinematic player stuff
    {
        // Update movement
        Vector3 moveDir = moveDir.ZERO;

        if (input->GetKeyDown(Key::KEY_W)) {
            moveDir += moveDir.FORWARD;
        }
        if (input->GetKeyDown(Key::KEY_S)) {
            moveDir += moveDir.BACK;
        }
        if (input->GetKeyDown(Key::KEY_A)) {
            moveDir += moveDir.LEFT;
        }
        if (input->GetKeyDown(Key::KEY_D)) {
            moveDir += moveDir.RIGHT;
        }

        // Normalize move vector so that diagonal strafing is not faster
        if (moveDir.LengthSquared() > 0.0f) {
            moveDir.Normalize();
        }

        // Walk
        kinematicController->SetWalkDirection(node->LocalToWorld(Vector4(moveDir * 0.06f, 0)));
    }

    return true;
}
}
