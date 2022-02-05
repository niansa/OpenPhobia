#pragma once

namespace Game {
class HouseBoundary;
}
#ifndef HOUSEBOUNDARY_HPP
#define HOUSEBOUNDARY_HPP
#include "../easyscript/Namespace.hpp"
#include "Useable.hpp"

#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/LogicComponent.h>



namespace Game {
class HouseBoundary final : public LogicComponent {
    URHO3D_OBJECT(HouseBoundary, LogicComponent);

public:
    using LogicComponent::LogicComponent;

};
}
#endif // PLAYER_HPP
