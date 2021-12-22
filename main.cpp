#include <iostream>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/PhysicsEvents.h>

using namespace Urho3D;




class App : public Application {
    URHO3D_OBJECT(App, Application);

    Scene *scene = nullptr;
    Node *level, *player, *head;

public:
    // Initialization
    explicit App(Context* context) : Application(context) {
        engineParameters_[EP_RESOURCE_PREFIX_PATHS] = "Project";
    }

    Node *loadObject(const eastl::string& name) {
        auto* cache = GetSubsystem<ResourceCache>();
        auto file = cache->GetResource<XMLFile>(name);

        if (file) {
            auto node = scene->CreateChild();
            node->LoadXML(file->GetRoot());
            return node;
        } else {
            //TODO
            abort();
        }
    }

    void loadScene(uint16_t levelNo) {
        if (scene) {
            scene->Remove();
        }

        scene = new Scene(context_);
        scene->CreateComponent<Octree>();

        level = loadObject("Scenes/level"+eastl::to_string(levelNo)+".xml");
        player = loadObject("Objects/Player.xml")->GetChild("Player");
        head = player->GetChild("Head");

        player->SetPosition(level->GetChild("Spawn")->GetPosition());

        auto* renderer = GetSubsystem<Renderer>();

        SharedPtr<Viewport> viewport(new Viewport(context_, scene, head->GetComponent<Camera>()));
        renderer->SetViewport(0, viewport);
    }

    void Start() final {
        SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(App, HandleUpdate));
        loadScene(1);
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
        if (player->GetPosition().y_ < -10) {
            loadScene(1);
            return;
        }

        // Camera
        auto mMove = input->GetMouseMove();
        if (mMove.x_ || mMove.y_) {
            { // Head rotation
                auto headRot = head->GetRotation().EulerAngles();
                headRot.x_ = Max(Min(headRot.x_ + mMove.y_ / 4.0f, 80), -80);
                head->SetRotation(Quaternion(headRot));
            }
            { // Body rotation
                player->Rotate({0, mMove.x_ / 4.0f, 0});
            }
        }

        // Movement
        auto playerBody = player->GetComponent<RigidBody>();
        auto playerVel = playerBody->GetLinearVelocity();
        if (input->GetKeyDown(Key::KEY_SPACE)) {
            if (playerVel.y_ > -0.5 && playerVel.y_ < 0.5) {
                eastl::vector<RigidBody*> collisions;
                playerBody->GetCollidingBodies(collisions);
                if (collisions.size() != 0) {
                    playerVel += player->GetWorldUp() * 7.5;
                }
            }
            playerVel += player->GetWorldDirection() * 2;
        } else if (input->GetKeyDown(Key::KEY_ESCAPE)) {
            exit(0);
        }
        limitVel(playerVel);
        playerBody->SetLinearVelocity(playerVel);
        auto bodyRot = player->GetRotation();
        bodyRot.x_ = 0;
        bodyRot.z_ = 0;
        player->SetRotation(bodyRot);
    }
};


URHO3D_DEFINE_APPLICATION_MAIN(App)
