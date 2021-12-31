#include "Player.hpp"
#include "../LevelManager.hpp"

#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/Camera.h>



namespace Game {
void Player::Start() {
    head = GetNode()->GetChild("Head");
    collisionShape = GetNode()->GetComponent<CollisionShape>();
    kinematicController = GetNode()->CreateComponent<KinematicCharacterController>();
    kinematicController->SetHeight(0.85f);

    auto* renderer = GetSubsystem<Renderer>();

    SharedPtr<Viewport> viewport(new Viewport(context_, GetScene(), head->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);
}

void Player::FixedUpdate(float timeStep) {
    auto* input = GetSubsystem<Input>();

    // Escape key handling
    if (input->GetKeyDown(Key::KEY_ESCAPE)) {
        exit(0);
    }

    // Position check
    if (GetNode()->GetWorldPosition().y_ < -10) {
        kinematicController->SetTransform(GetNode()->GetParent()->GetWorldPosition(), {0, 0, 0});
        return;
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
            GetNode()->Rotate({0, mMove.x_ / 4.0f, 0});
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
        kinematicController->SetWalkDirection(GetNode()->GetRotation() * moveDir * 0.03f);
    }
}
}
