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
class RoomBoundary : public LogicComponent {
    URHO3D_OBJECT(RoomBoundary, LogicComponent);

    BoundingBox bBox;
    LevelManager *levelManager;

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

    bool isNodeInside(Node *otherNode);
};
}
#endif // ROOMBOUNDARY_HPP
