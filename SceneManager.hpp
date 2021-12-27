namespace Scatter {
class SceneManager;
}
#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP
#include "Namespace.hpp"
#include "FriendlyApp.hpp"

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/Camera.h>



namespace Scatter {
struct SceneManager {
    FriendlyApp *app;
    Urho3D::Scene *scene = nullptr;
    Urho3D::Node *level, *player, *head;

    SceneManager(FriendlyApp *app) : app(app) {}

    Urho3D::Node *loadObject(const eastl::string& name);

    void loadScene(uint16_t levelNo);
};
}
#endif // SCENEMANAGER_HPP
