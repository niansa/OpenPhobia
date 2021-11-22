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
    Node *player;

public:
    // Initialization
    explicit App(Context* context) : Application(context) {
        engineParameters_[EP_RESOURCE_PREFIX_PATHS] = "Project";
    }

    void loadScene(uint16_t level) {
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

        auto* renderer = GetSubsystem<Renderer>();

        SharedPtr<Viewport> viewport(new Viewport(context_, scene, player->GetComponent<Camera>()));
        renderer->SetViewport(0, viewport);
    }

    void Start() final {
        SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(App, HandleUpdate));
        loadScene(1);
    }

    // Runtime
    void HandleUpdate(StringHash, VariantMap&) {
        if (player->GetPosition().y_ < -10) {
            // TODO: Reset player here (by reloading scene?)
        }
    }

};


URHO3D_DEFINE_APPLICATION_MAIN(App)
