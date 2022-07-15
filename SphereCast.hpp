#ifndef _SPHERECAST_HPP
#define _SPHERECAST_HPP
#include "easyscript/Namespace.hpp"

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Math/Vector3.h>



namespace Game {
struct SphereCastResult {
    Node *node;
    float distance;
};

eastl::vector<SphereCastResult> SphereCast(Scene *scene, const Vector3& position, float radius);
}
#endif
