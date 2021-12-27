namespace Scatter {
class Main;
}
#ifndef MAIN_HPP
#define MAIN_HPP
#include "Namespace.hpp"
#include "LevelManager.hpp"
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
class Main final : public FriendlyApp {
    friend SceneManager;

    URHO3D_OBJECT(Main, Application);

    LevelManager levelManager;

public:
    // Initialization
    explicit Main(Context* context) : FriendlyApp(context), levelManager(this) {
        engineParameters_[EP_RESOURCE_PREFIX_PATHS] = "Project";
    }

    virtual void Start() override {
        levelManager.Start();
        SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Main, Update));
    }

    void Update(StringHash, VariantMap&) {
        levelManager.Update();
    }
};
}
#endif // MAIN_HPP
