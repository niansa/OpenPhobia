#include "SphereCast.hpp"
#include "easyscript/Namespace.hpp"

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Math/Vector3.h>



namespace Game {
eastl::vector<SphereCastResult> SphereCast(Scene *scene, const Vector3& position, float radius) {
    eastl::vector<SphereCastResult> fres;
    for (auto& node : scene->GetChildren(true)) {
        auto distance = (node->GetWorldPosition() - position).Length();
        if (distance < radius) {
            fres.push_back({node, distance});
        }
    }
    return fres;
}
}
