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
#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/PhysicsEvents.h>

using namespace Urho3D;




class App : public Application {
    URHO3D_OBJECT(App, Application);

    Scene *scene = nullptr;
    Node *player, *goal;
    uint level = 1;

public:
    // Initialization
    explicit App(Context* context) : Application(context) {
        engineParameters_[EP_RESOURCE_PREFIX_PATHS] = "Project";
    }

    void loadScene(uint level) {
        auto* cache = GetSubsystem<ResourceCache>();

        if (scene) {
            scene->Remove();
        }

        scene = new Scene(context_);

        SharedPtr<File> file = cache->GetFile("Scenes/level"+eastl::to_string(level)+".xml");
        if (file) {
            scene->LoadXML(*file);
        } else {
            //TODO
            abort();
        }

        player = scene->GetChild("Player");
        goal = scene->GetChild("Goal");

        auto* renderer = GetSubsystem<Renderer>();

        SharedPtr<Viewport> viewport(new Viewport(context_, scene, player->GetComponent<Camera>()));
        renderer->SetViewport(0, viewport);
    }

    void Start() final {
        // Handlers
        SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(App, HandleUpdate));
        SubscribeToEvent(E_NODECOLLISION, URHO3D_HANDLER(App, HandleNodeCollision));

        // Load scene
        loadScene(level);

        // Configure graphics
        {
            auto* graphics = GetSubsystem<Graphics>();

            auto params = graphics->GetScreenModeParams();
            params.vsync_ = true;
            graphics->SetScreenMode(graphics->GetWidth(), graphics->GetHeight(), params);
        }
    }

    // Runtime
    void HandleUpdate(StringHash, VariantMap&) {
        auto* input = GetSubsystem<Input>();

        // Checks
        if (player->GetPosition().y_ < -10) {
            // Reload scene
            loadScene(level);
        }

        // Controls
        {
            // Movement
            if (input->GetKeyDown(KEY_W) || input->GetKeyDown(KEY_S)) {
                auto rigid = player->GetComponent<RigidBody>();
                auto impulse = player->GetDirection()/4;
                // Never apply up-/downwards impulse
                impulse.y_ = 0;
                // Negate if backwards
                if (input->GetKeyDown(KEY_S)) {
                    impulse = -impulse;
                }
                // Apply
                rigid->ApplyImpulse(impulse);
            }
            if (input->GetKeyDown(KEY_A)) {
                player->Rotate({0, -2.5, 0});
            }
            if (input->GetKeyDown(KEY_D)) {
                player->Rotate({0, 2.5, 0});
            }
            if (input->GetKeyDown(KEY_Q)) {
                player->Rotate({-2.5, 0, 0});
            }
            if (input->GetKeyDown(KEY_E)) {
                player->Rotate({2.5, 0, 0});
            }
        }
    }

    void HandleNodeCollision(StringHash eventType, VariantMap& eventData) {
        auto otherNode = eventData["OtherNode"].GetPtr();

        if (otherNode == goal) {
            loadScene(++level);
        }
    }
};


URHO3D_DEFINE_APPLICATION_MAIN(App)
