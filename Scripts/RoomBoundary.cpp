#include "RoomBoundary.hpp"
#include "../LevelManager.hpp"
#include "Player.hpp"
#include "Lightbulb.hpp"

#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Zone.h>



namespace Game {
void RoomBoundary::Start() {
    if (GetNode()->HasComponent<StaticModel>()) {
        bBox = GetNode()->GetComponent<StaticModel>()->GetWorldBoundingBox();
    } else if (GetNode()->HasComponent<Zone>()) {
        bBox = GetNode()->GetComponent<Zone>()->GetWorldBoundingBox();
    }
    levelManager = GetGlobalVar("LevelManager").GetCustom<LevelManager*>();

    // Get lightbulbs in room
    ea::vector<Node*> lightbulbNodes;
    GetScene()->GetChildrenWithComponent<Lightbulb>(lightbulbNodes, true);
    for (const auto& node : lightbulbNodes) {
        if (isPosInside(node->GetWorldPosition())) {
            lightbulbs.push_back(node->GetComponent<Lightbulb>());
        }
    }
}

bool RoomBoundary::isPosInside(Vector3 pos) {
    return bBox.IsInside(pos) == Intersection::INSIDE;
}

bool RoomBoundary::isNodeInside(Node *otherNode) {
    if (otherNode->HasComponent<StaticModel>()) {
        auto isInside = bBox.IsInside(otherNode->GetComponent<StaticModel>()->GetBoundingBox());
        return isInside == Intersection::INSIDE || Intersection::INSIDE;
    } else {
        return isPosInside(otherNode->GetWorldPosition());
    }
}

bool RoomBoundary::areAllLightsOff() {
    for (const auto& lightbulb : lightbulbs) {
        if (lightbulb->isTurnedOn()) {
            return false;
        }
    }
    return true;
}
}
