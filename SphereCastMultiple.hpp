#include "easyscript/Namespace.hpp"

#include <Urho3D/Physics/PhysicsWorld.h>



namespace Game {
void SphereCastMultiple(PhysicsWorld* pw, eastl::vector<PhysicsRaycastResult>& results, const Ray& ray, float radius, float maxDistance, unsigned collisionMask = M_MAX_UNSIGNED);
}
