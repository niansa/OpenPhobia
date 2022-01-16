#include "easyscript/Namespace.hpp"

#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Math/Ray.h>



namespace Game {
void SphereCastMultiple(PhysicsWorld* pw, eastl::vector<PhysicsRaycastResult>& results, const Ray& ray, float radius, float maxDistance, unsigned collisionMask = M_MAX_UNSIGNED);
inline void SphereCastMultipleDS(PhysicsWorld* pw, eastl::vector<PhysicsRaycastResult>& results, const Ray& ray, float radius, float maxDistance, unsigned collisionMask = M_MAX_UNSIGNED) {
    Ray nRay = ray;
    nRay.direction_ = -nRay.direction_;
    SphereCastMultiple(pw, results, ray, radius, maxDistance, collisionMask);
    SphereCastMultiple(pw, results, nRay, radius, maxDistance, collisionMask);
}
}
