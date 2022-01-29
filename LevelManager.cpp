#include "LevelManager.hpp"
#include "easyscript/Namespace.hpp"
#include "Scripts/Player.hpp"
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

    // Crosshair  TODO: Fixup
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

    // Find important components
    eastl::vector<Node*> nodes;
    scene->GetNodesWithTag(nodes, "Player");
    for (auto node : nodes) {
        players.push_back(node->GetComponent<Player>());
    }
    nodes.clear();
    scene->GetNodesWithTag(nodes, "Ghost");
    ghost = nodes[0]->GetComponent<Ghost>();

    // Load ghost stuff  TODO: Based on difficulty
    ghost->setGhostBehavior({30});
}

unsigned LevelManager::getTeamSanity() const {
    if (!players.empty()) {
        unsigned fres = 0;
        for (const auto player : players) {
            fres += player->sanity;
        }
        return fres / players.size();
    } else {
        return 100;
    }
}
}
