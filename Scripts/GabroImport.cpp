#include "GabroImport.hpp"
#include "../LevelManager.hpp"
#include "EMFEmitter.hpp"

#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>



namespace Game {
void GabroImport::Start() {
    // Get resource cache
    auto resCache = GetSubsystem<ResourceCache>();
    // Get scene
    auto scene = GetScene()->GetChild("SceneMain");
    // Create rooms
    for (const auto& rProb : scene->GetChild("Rooms")->GetChildren()) {
        rProb->CreateComponent("RoomBoundary");
    }
}
}
