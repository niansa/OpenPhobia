#include "LevelManager.hpp"
#include "easyscript/Namespace.hpp"
#include "Scripts/Player.hpp"
#include "Scripts/Ghost.hpp"
#include "Scripts/RoomBoundary.hpp"
#include "Scripts/HouseBoundary.hpp"
#include "GhostBehavior.hpp"
#include "GhostIdentity.hpp"

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
}

void LevelManager::loadEnv() {
    eastl::vector<Node*> nodes;
    // Find players
    scene->GetNodesWithTag(nodes, "Player");
    for (auto node : nodes) {
        players.push_back(node->GetComponent<Player>());
    }
    nodes.clear();

    // Find ghost and configure ghost if found
    scene->GetNodesWithTag(nodes, "Ghost");
    if (!nodes.empty()) {
        // Get ghost
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

        // Load ghost identity
        ghostIdentitySeed = rng.GetUInt();
        ghostIdentity = GhostIdentity(app->GetContext(), ghostIdentitySeed);

        // Load ghost AI stuff  TODO: Based on difficulty
        auto ghostBehavior = getGhostBehavior(ghostIdentity.type);
        ghostBehavior->ghost = ghost;
        ghost->setGhostBehavior(std::move(ghostBehavior));
    } else {
        ghost = nullptr;
    }
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

RoomBoundary *LevelManager::getPosRoom(Vector3 pos) {
    for (auto room : rooms) {
        if (room->isPosInside(pos)) {
            return room;
        }
    }
    return nullptr;
}
}
