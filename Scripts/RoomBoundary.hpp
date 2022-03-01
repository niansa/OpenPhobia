#pragma once

namespace Game {
class RoomBoundary;
}
#ifndef ROOMBOUNDARY_HPP
#define ROOMBOUNDARY_HPP
#include "../easyscript/Namespace.hpp"
#include "../LevelManager.hpp"

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Math/BoundingBox.h>



namespace Game {
class Lightbulb;

class RoomBoundary : public LogicComponent {
    URHO3D_OBJECT(RoomBoundary, LogicComponent);

    BoundingBox bBox;
    LevelManager *levelManager;
    eastl::vector<Lightbulb*> lightbulbs;

public:
    using LogicComponent::LogicComponent;

    void Start() override;

    Node *getRoom() {
        return GetNode()->GetParent();
    }
    eastl::string getRoomName() {
        if (this) {
            return getRoom()->GetName();
        } else {
            return "Hallway";
        }
    }
    const BoundingBox &getBoundingBox() {
        return bBox;
    }
    const eastl::vector<Lightbulb*> &getLightbulbs() {
        return lightbulbs;
    }

    bool isPosInside(Vector3 pos);
    bool isNodeInside(Node *otherNode);
    bool areAllLightsOff();
};
}
#endif // ROOMBOUNDARY_HPP
