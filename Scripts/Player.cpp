#include "Player.hpp"
#include "Useable.hpp"
#include "Door.hpp"
#include "RoomBoundary.hpp"
#include "HouseBoundary.hpp"
#include "../LevelManager.hpp"

#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/OctreeQuery.h>
#include <Urho3D/Audio/Audio.h>
#include <Urho3D/Audio/SoundListener.h>
#include <Urho3D/UI/UI.h>



namespace Game {
void Player::Start() {
    head = GetNode()->GetChild("Head");
    handP = head->GetChild("Hand");
    levelManager = GetGlobalVar("LevelManager").GetCustom<LevelManager*>();
    collisionShape = GetNode()->GetComponent<CollisionShape>();
    kinematicController = GetNode()->CreateComponent<KinematicCharacterController>();
    kinematicController->SetHeight(GetNode()->GetScale().y_);
    kinematicController->SetCollisionLayerAndMask(1, 1);

    auto* renderer = GetSubsystem<Renderer>();

    // Set viewport camera
    SharedPtr<Viewport> viewport(new Viewport(context_, GetScene(), head->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);

    // Set audio audio listener
    GetSubsystem<Audio>()->SetListener(head->GetComponent<SoundListener>());
}

void Player::FixedUpdate(float timeStep) {
    auto input = GetSubsystem<Input>();

    // Escape key handling
    if (input->GetKeyDown(Key::KEY_ESCAPE)) {
        exit(0);
    }

    // Position check
    if (GetNode()->GetWorldPosition().y_ < -10) {
        kinematicController->SetTransform(GetNode()->GetParent()->GetWorldPosition(), {0, 0, 0});
        return;
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
        kinematicController->SetWalkDirection(GetNode()->GetRotation() * moveDir * walkSpeed / 60.0f);
    }
}

void Player::Update(float timeStep) {
    auto* input = GetSubsystem<Input>();

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

    // Object grabbing
    if (!hand && input->GetKeyPress(Key::KEY_E)) {
        // Grab item
        auto *ui = GetSubsystem<UI>();
        auto *graphics = GetSubsystem<Graphics>();
        IntVector2 pos = ui->GetCursorPosition();
        // Raycast
        auto ray = head->GetComponent<Camera>()->GetScreenRay(float(pos.x_) / graphics->GetWidth(), float(pos.y_) / graphics->GetHeight());
        ea::vector<RayQueryResult> results;
        RayOctreeQuery query(results, ray, RAY_TRIANGLE, grabRange, DRAWABLE_GEOMETRY);
        GetScene()->GetComponent<Octree>()->RaycastSingle(query);
        // Get first result
        if (!results.empty()) {
            auto node = results[0].node_;
            while (node) {
                if (node->HasTag("Grabbable")) {
                    // Grab obect
                    grab(node);
                    break;
                }
                // Retry with parent node
                node = node->GetParent();
            }
        }
    }

    // Object dropping
    if (hand && input->GetKeyPress(Key::KEY_G)) {
        drop();
    }

    // Object use
    if (hand && input->GetMouseButtonPress(MouseButtonFlags::Enum::MOUSEB_RIGHT) && hand->HasTag("Useable")) {
        auto script = static_cast<Useable *>(hand->GetComponent(hand->GetName()));
        script->Use();
    }
    if (input->GetMouseButtonPress(MouseButtonFlags::Enum::MOUSEB_LEFT)) {
        auto *ui = GetSubsystem<UI>();
        auto *graphics = GetSubsystem<Graphics>();
        IntVector2 pos = ui->GetCursorPosition();
        // Raycast
        auto ray = head->GetComponent<Camera>()->GetScreenRay(float(pos.x_) / graphics->GetWidth(), float(pos.y_) / graphics->GetHeight());
        ea::vector<RayQueryResult> results;
        RayOctreeQuery query(results, ray, RAY_TRIANGLE, interactRange, DRAWABLE_GEOMETRY);
        GetScene()->GetComponent<Octree>()->RaycastSingle(query);
        // Get first result
        if (!results.empty()) {
            auto node = results[0].node_;
            while (node) {
                if (node->HasTag("Useable") && !node->HasTag("Grabbable")) {
                    // Use object
                    auto script = static_cast<Useable *>(node->GetComponent(node->GetName()));
                    script->Use();
                    break;
                }
                // Retry with parent node
                node = node->GetParent();
            }
        }
    }

    // Drag doors
    if (input->GetMouseButtonDown(MouseButtonFlags::Enum::MOUSEB_LEFT)) {
        auto *ui = GetSubsystem<UI>();
        auto *graphics = GetSubsystem<Graphics>();
        IntVector2 pos = ui->GetCursorPosition();
        // Raycast
        auto ray = head->GetComponent<Camera>()->GetScreenRay(float(pos.x_) / graphics->GetWidth(), float(pos.y_) / graphics->GetHeight());
        ea::vector<RayQueryResult> results;
        RayOctreeQuery query(results, ray, RAY_TRIANGLE, interactRange, DRAWABLE_GEOMETRY);
        GetScene()->GetComponent<Octree>()->RaycastSingle(query);
        // Get first result
        if (!results.empty()) {
            auto node = results[0].node_->GetParent();
            // Get door
            if (node->HasComponent<Door>()) {
                auto door = node->GetComponent<Door>();
                door->push(1.0f);
            }
        }
    }
}

void Player::grab(Node *node) {
    handP->AddChild(node);
    auto body = node->GetComponent<RigidBody>();
    body->SetKinematic(true);
    body->SetTrigger(true);
    node->SetTransform(Vector3::ZERO, Quaternion(node->HasTag("xRot")?Vector3{90, 0, 0}:Vector3::ZERO));
    hand = node;
}

void Player::drop() {
    // Drop
    GetScene()->AddChild(hand);
    auto body = hand->GetComponent<RigidBody>();
    body->SetPosition(head->GetWorldPosition());
    body->SetKinematic(false);
    body->SetTrigger(false);
    body->SetLinearVelocity(head->GetWorldDirection()*4);
    hand = nullptr;
}

RoomBoundary *Player::getCurrentRoom() {
    return levelManager->getNodeRoom(GetNode());
}

bool Player::isInsideHouse() {
    return levelManager->getHouse()->isNodeInside(GetNode());
}
}
