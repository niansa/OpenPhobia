#include "LevelManager.hpp"
#include "easyscript/Namespace.hpp"
#include "Scripts/Player.hpp"
#include "Scripts/Ghost.hpp"
#include "Scripts/RoomBoundary.hpp"
#include "Scripts/HouseBoundary.hpp"
#include "GhostBehavior.hpp"

#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/Math/RandomEngine.h>
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

    // Load crosshair texture
    auto cache = app->GetSubsystem<ResourceCache>();
    auto crosshairTextureFile = cache->GetResource<Texture2D>("Textures/crosshair.png");
    if (crosshairTextureFile) {
        crosshair->SetSize(crosshairTextureFile->GetSize() / 8);
        crosshair->SetPosition(-(crosshair->GetSize().x_ / 2), -(crosshair->GetSize().y_ / 2));
        crosshair->SetTexture(crosshairTextureFile);
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
    nodes.clear();
    scene->GetNodesWithTag(nodes, "Scripted");
    for (auto node : nodes) {
        if (node->HasComponent<RoomBoundary>()) {
            rooms.push_back(node->GetComponent<RoomBoundary>());
        } else if (node->HasComponent<HouseBoundary>()) {
            house = node->GetComponent<HouseBoundary>();
        }
    }

    // Load ghost stuff  TODO: Based on difficulty
    auto ghostBehavior = getGhostBehavior(GhostType(RandomEngine().GetUInt(unsigned(GhostType::_lowest), unsigned(GhostType::_highest))));
    ghostBehavior->ghost = ghost;
    ghost->setGhostBehavior(std::move(ghostBehavior));
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

bool LevelManager::isAnyPlayerInHouse() {
    for (auto player : players) {
        if (player->isInsideHouse()) {
            return true;
        }
    }
    return false;
}

RoomBoundary *LevelManager::getNodeRoom(Node *node) {
    for (auto room : rooms) {
        if (room->isNodeInside(node)) {
            return room;
        }
    }
    return nullptr;
}
}
