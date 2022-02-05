#include "RoomBoundary.hpp"
#include "../LevelManager.hpp"
#include "Player.hpp"

#include <Urho3D/Graphics/StaticModel.h>



namespace Game {
void RoomBoundary::Start() {
    bBox = GetNode()->GetComponent<StaticModel>()->GetWorldBoundingBox();
    levelManager = GetGlobalVar("LevelManager").GetCustom<LevelManager*>();
}

bool RoomBoundary::isNodeInside(Node *otherNode) {
    return bBox.IsInside(otherNode->GetWorldPosition()) == Intersection::INSIDE;
}
}
