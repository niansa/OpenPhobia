#pragma once

namespace Game {
class HouseBoundary;
}
#ifndef HOUSEBOUNDARY_HPP
#define HOUSEBOUNDARY_HPP
#include "../easyscript/Namespace.hpp"
#include "RoomBoundary.hpp"

#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/LogicComponent.h>



namespace Game {
class HouseBoundary final : public RoomBoundary {
    URHO3D_OBJECT(HouseBoundary, RoomBoundary);

public:
    using RoomBoundary::RoomBoundary;

};
}
#endif // HOUSEBOUNDARY_HPP
