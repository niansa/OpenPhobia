#include "SceneManager.hpp"

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Resource/XMLFile.h>



namespace Scatter {
Node *SceneManager::loadObject(const eastl::string& name) {
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

void SceneManager::loadScene(uint16_t levelNo) {
    if (scene) {
        scene->Remove();
    }

    scene = new Scene(app->context_);
    scene->CreateComponent<Octree>();

    level = loadObject("Scenes/level"+eastl::to_string(levelNo)+".xml");
    player = loadObject("Objects/Player.xml")->GetChild("Player");
    head = player->GetChild("Head");

    player->SetPosition(level->GetChild("Spawn")->GetPosition());

    auto* renderer = app->GetSubsystem<Renderer>();

    SharedPtr<Viewport> viewport(new Viewport(app->context_, scene, head->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);
}
}
