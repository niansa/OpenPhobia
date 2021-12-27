#include "SceneManager.hpp"

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Resource/XMLFile.h>



namespace Scatter {
Node *SceneManager::loadNode(const eastl::string& name) {
    auto* cache = app->GetSubsystem<ResourceCache>();
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

void SceneManager::loadScene(const eastl::string& filename) {
    if (scene) {
        scripts.clear();
        scene->Remove();
    }

    scene = new Scene(app->context_);
    scene->CreateComponent<Octree>();

    level = loadNode(filename);

    // Load player
    player = loadObject<Player>("Objects/Player.xml", "Player");
    player->node->SetPosition(level->GetChild("Spawn")->GetPosition());

    auto* renderer = app->GetSubsystem<Renderer>();

    SharedPtr<Viewport> viewport(new Viewport(app->context_, scene, player->getHead()->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);
}

void SceneManager::Update() {
    for (const auto& script : scripts) {
        if (!script->Update()) break;
    }
}
}
