#pragma once

namespace Game {
class Equipment;
}
#ifndef EQUIPMENT_HPP
#define EQUIPMENT_HPP
#include "Useable.hpp"
#include "LevelManager.hpp"
#include "easyscript/Namespace.hpp"
#include "Scripts/Ghost.hpp"

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/Scene/Node.h>



namespace Game {
class Equipment : public Useable {
protected:
    LevelManager *levelManager = nullptr;

public:
    using Useable::Useable;

    LevelManager *getLevelManager() {
        if (!levelManager) {
            levelManager = GetGlobalVar("LevelManager").GetCustom<LevelManager*>();
        }
        return levelManager;
    }
    Ghost *getGhost() {
        return getLevelManager()->getGhost();
    }
    float getDistanceToGhost() {
        auto ghostNode = getGhost()->GetNode();
        return (ghostNode->GetWorldPosition() - GetNode()->GetWorldPosition()).Length();
    }
    bool isDistorted() {
        return getDistanceToGhost() < 10.0f && getGhost()->isVisible();
    }
};
}
#endif
