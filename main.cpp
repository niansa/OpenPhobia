#include <iostream>
#include <random>
#include <Urho3D/Urho3DAll.h>

using namespace Urho3D;




class App : public Application {
    URHO3D_OBJECT(App, Application);

    Scene *scene;
    Node *player;

public:
    // Initialization
    explicit App(Context* context) : Application(context) {}

    void Start() final {
        srand(1765783456);
        auto* cache = GetSubsystem<ResourceCache>();

        // Handlers
        SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(App, HandleUpdate));

        // Load scene
        {
            scene = new Scene(context_);

            SharedPtr<File> file = cache->GetFile("Scenes/level1.xml");
            scene->LoadXML(*file);

            player = scene->GetChild("Player");
        }

        // Configure renderer
        {
            auto* renderer = GetSubsystem<Renderer>();
            auto* graphics = GetSubsystem<Graphics>();

            SharedPtr<Viewport> viewport(new Viewport(context_, scene, player->GetComponent<Camera>()));
            renderer->SetViewport(0, viewport);
            auto params = graphics->GetScreenModeParams();
            params.vsync_ = true;
            graphics->SetScreenMode(graphics->GetWidth(), graphics->GetHeight(), params);
        }
    }

    // Runtime
    void HandleUpdate(StringHash, VariantMap&) {
        auto* input = GetSubsystem<Input>();

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
};


URHO3D_DEFINE_APPLICATION_MAIN(App)
