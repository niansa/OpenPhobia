#pragma once

namespace Game {
class RoomBoundary;
}
#ifndef ROOMBOUNDARY_HPP
#define ROOMBOUNDARY_HPP
#include "../easyscript/Namespace.hpp"
#include "Useable.hpp"

#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/LogicComponent.h>



namespace Game {
class RoomBoundary final : public LogicComponent {
    URHO3D_OBJECT(RoomBoundary, LogicComponent);

public:
    using LogicComponent::LogicComponent;

};
}
#endif // PLAYER_HPP
