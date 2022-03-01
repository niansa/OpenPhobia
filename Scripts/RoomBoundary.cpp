#include "RoomBoundary.hpp"
#include "../LevelManager.hpp"
#include "Player.hpp"
#include "Lightbulb.hpp"

#include <Urho3D/Graphics/StaticModel.h>



namespace Game {
void RoomBoundary::Start() {
    bBox = GetNode()->GetComponent<StaticModel>()->GetWorldBoundingBox();
    levelManager = GetGlobalVar("LevelManager").GetCustom<LevelManager*>();

    // Get lightbulbs in room
    ea::vector<Node*> lightbulbNodes;
    GetScene()->GetChildrenWithComponent<Lightbulb>(lightbulbNodes, true);
    for (const auto& node : lightbulbNodes) {
        if (isNodeInside(node)) {
            lightbulbs.push_back(node->GetComponent<Lightbulb>());
        }
    }
}

bool RoomBoundary::isPosInside(Vector3 pos) {
    return bBox.IsInside(pos) == Intersection::INSIDE;
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
