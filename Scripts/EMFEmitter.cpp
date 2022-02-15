#include "EMFEmitter.hpp"

#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>



namespace Game {
void EMFEmitter::FixedUpdate(float) {
    if (timeout) {
        if (timer.GetMSec(false) > timeout) {
            DoAutoRemove(removeMode);
        }
    }
}

EMFEmitter *EMFEmitter::spawnSpot(Scene *scene, const Vector3& position) {
    auto node = scene->CreateChild();
    node->SetWorldPosition(position);
    auto body = node->CreateComponent<RigidBody>();
    auto shape = node->CreateComponent<CollisionShape>();
    body->SetCollisionLayerAndMask(10, 1);
    shape->SetBox({0.1f, 0.1f, 0.1f});
    auto emitter = node->CreateComponent<EMFEmitter>();
    emitter->setAutoRemoveMode(AutoRemoveMode::REMOVE_NODE);
    return emitter;
}
}
