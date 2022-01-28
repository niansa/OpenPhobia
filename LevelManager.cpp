#include "LevelManager.hpp"
#include "easyscript/Namespace.hpp"
#include "Scripts/Ghost.hpp"

#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Texture2D.h>



namespace Game {
void LevelManager::Start() {
    Variant v;
    v.SetCustom<LevelManager*>(this);
    app->SetGlobalVar("LevelManager", v);
    reloadLevel();

    // Crosshair
    auto ui = app->GetSubsystem<UI>();
    auto crosshair = ui->GetRoot()->CreateChild<Sprite>("crosshair");
    crosshair->SetAlignment(HorizontalAlignment::HA_CENTER, VerticalAlignment::VA_CENTER);

    // Load crosshair texture  TODO: Fixup
    auto cache = app->GetSubsystem<ResourceCache>();
    auto crosshairTextureFile = cache->GetResource<Texture2D>("Textures/crosshair.png");
    if (crosshairTextureFile) {
        crosshair->SetTexture(crosshairTextureFile);
    } else {
        //TODO
        abort();
    }

    // Find ghost
    for (auto node : scene->GetChildren(true)) {
        if (node->HasComponent<Ghost>()) {
            ghost = node->GetComponent<Ghost>();
            break;
        }
    }
}
}
