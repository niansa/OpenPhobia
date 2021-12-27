namespace Scatter {
class Main;
}
#ifndef MAIN_HPP
#define MAIN_HPP
#include "Namespace.hpp"
#include "SceneManager.hpp"
#include "FriendlyApp.hpp"

#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/PhysicsEvents.h>



namespace Scatter {
class Main : public FriendlyApp {
    friend SceneManager;

    URHO3D_OBJECT(Main, Application);

    SceneManager sceneManager;

public:
    // Initialization
    explicit Main(Context* context) : FriendlyApp(context), sceneManager(this) {
        engineParameters_[EP_RESOURCE_PREFIX_PATHS] = "Project";
    }

    void Start() final {
        SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Main, HandleUpdate));
        sceneManager.loadScene(1);
    }

    // Utilities
    void limitVel(Vector3& vel) {
        vel.x_ = Max(Min(vel.x_, 10), -10);
        vel.y_ = Max(Min(vel.y_, 10), -100);
        vel.z_ = Max(Min(vel.z_, 10), -10);
    }

    // Runtime
    void HandleUpdate(StringHash, VariantMap&) {
        auto* input = GetSubsystem<Input>();

        // Position check
        if (sceneManager.player->GetPosition().y_ < -10) {
            sceneManager.loadScene(1);
            return;
        }

        // Camera
        auto mMove = input->GetMouseMove();
        if (mMove.x_ || mMove.y_) {
            { // Head rotation
                auto headRot = sceneManager.head->GetRotation().EulerAngles();
                headRot.x_ = Max(Min(headRot.x_ + mMove.y_ / 8.0f, 80), -80);
                sceneManager.head->SetRotation(Quaternion(headRot));
            }
            { // Body rotation
                sceneManager.player->Rotate({0, mMove.x_ / 4.0f, 0});
            }
        }

        // Movement
        auto playerBody = sceneManager.player->GetComponent<RigidBody>();
        auto playerVel = playerBody->GetLinearVelocity();
        if (input->GetKeyDown(Key::KEY_SPACE)) {
            if (playerVel.y_ > -0.5 && playerVel.y_ < 0.5) {
                eastl::vector<RigidBody*> collisions;
                playerBody->GetCollidingBodies(collisions);
                if (collisions.size() != 0) {
                    playerVel += sceneManager.player->GetWorldUp() * 7.5;
                }
            }
            playerVel += sceneManager.player->GetWorldDirection() / 8;
        } else if (input->GetKeyDown(Key::KEY_ESCAPE)) {
            exit(0);
        }
        limitVel(playerVel);
        playerBody->SetLinearVelocity(playerVel);
        auto bodyRot = sceneManager.player->GetRotation();
        bodyRot.x_ = 0;
        bodyRot.z_ = 0;
        sceneManager.player->SetRotation(bodyRot);
    }
};
}
#endif // MAIN_HPP
