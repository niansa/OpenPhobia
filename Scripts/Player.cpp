#include "Player.hpp"
#include "../LevelManager.hpp"



namespace Game {
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

    // Movement
    auto playerBody = node->GetComponent<RigidBody>();
    auto playerVel = playerBody->GetLinearVelocity();
    if (input->GetKeyDown(Key::KEY_SPACE)) {
        if (playerVel.y_ > -0.5 && playerVel.y_ < 0.5) {
            eastl::vector<RigidBody*> collisions;
            playerBody->GetCollidingBodies(collisions);
            if (collisions.size() != 0) {
                playerVel += node->GetWorldUp() * 7.5;
            }
        }
        playerVel += node->GetWorldDirection() / 8;
    }
    limitVel(playerVel);
    playerBody->SetLinearVelocity(playerVel);
    auto bodyRot = node->GetRotation();
    bodyRot.x_ = 0;
    bodyRot.z_ = 0;
    node->SetRotation(bodyRot);

    // Shooting
    if (input->GetMouseButtonPress(MouseButtonFlags::Enum::MOUSEB_LEFT)) {
        auto bullet = node->GetParent()->CreateChild();
        bullet->SetName("Spawn_Bullet");
        bullet->SetPosition(node->GetPosition()+getHead()->GetPosition());
        bullet->Translate(getHead()->GetWorldDirection());
        bullet->SetScale({0.25, 0.25, 0.25});
        lMan->loadSpawner(bullet);
        bullet->GetChild("Bullet")->GetComponent<RigidBody>()->ApplyImpulse(getHead()->GetWorldDirection()*10);
        return false;
    }

    return true;
}
}
