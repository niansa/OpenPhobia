#include "Jumper.hpp"
#include "../LevelManager.hpp"



namespace Game {
bool Jumper::Update() {
    auto body = node->GetComponent<RigidBody>();
    body->ApplyImpulse({0, 0.05, 0});
    return true;
}
}
