#ifndef _SPHERECAST_HPP
#define _SPHERECAST_HPP
#include "easyscript/Namespace.hpp"

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Math/Vector3.h>



namespace Game {
eastl::vector<Node*> SphereCast(Scene *scene, const Vector3& position, float radius);
}
#endif
