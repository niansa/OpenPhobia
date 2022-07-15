#include "SphereCast.hpp"
#include "easyscript/Namespace.hpp"

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Math/Vector3.h>



namespace Game {
eastl::vector<Node*> SphereCast(Scene *scene, const Vector3& position, float radius) {
    eastl::vector<Node*> fres;
    for (auto& node : scene->GetChildren(true)) {
        if ((node->GetWorldPosition() - position).Length() < radius) {
            fres.push_back(node);
        }
    }
    return fres;
}
}
