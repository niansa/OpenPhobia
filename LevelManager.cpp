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
#include <Urho3D/Core/Assert.h>
#include <Urho3D/RenderPipeline/RenderPipeline.h>
#include <Urho3D/RenderPipeline/BloomPass.h>
#include <Urho3D/RenderPipeline/SceneProcessor.h>



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

void LevelManager::reloadLevel() {
    LoadScene("Scenes/"+level+".xml");
    // Apply graphics settings
    auto renderPipeline = scene->GetOrCreateComponent<RenderPipeline>();
    auto renderSettings = renderPipeline->GetSettings();
    renderSettings.bloom_.enabled_ = false;
    renderSettings.bloom_.intensity_ = 1.0f;
    renderSettings.sceneProcessor_.lightingMode_ = DirectLightingMode::Forward;
    renderSettings.sceneProcessor_.maxPixelLights_ = 10;
    renderSettings.renderBufferManager_.colorSpace_ = RenderPipelineColorSpace::LinearLDR;
    //renderSettings.chromaticAberration_ = 0.25f;
    renderPipeline->SetSettings(renderSettings);
    // Load environment
    loadEnv();
}

void LevelManager::loadEnv() {
    eastl::vector<Node*> nodes;

    // Upgrade scene
    scene->GetChildren(nodes, true);
    const eastl::string_view spawner_prefix = "Spawn_";
    bool converted = false;
    for (auto node : nodes) {
        // Find spawners
        if (node->GetName().starts_with(spawner_prefix)) {
            converted = true;
            auto nname = eastl::string_view{node->GetName().data() + spawner_prefix.size(), node->GetName().size() - spawner_prefix.size()};
            // Convert
            node->SetName(eastl::string(nname));
            printf("Converting spawner for %s\n...", node->GetName().c_str());
            node->AddTag("Spawner");
            node->SetVar("Object", node->GetName());
        }
        // Find scripted nodes
        else if (node->HasTag("Scripted")) {
            converted = true;
            eastl::string nname = node->GetName();
            // Convert
            printf("Converting spawner for %s\n...", nname.c_str());
            node->SetVar("Object", eastl::move(nname));
        }
    }
    if (converted) {
        printf("Serializing converted scene...\n");
        fflush(stdout);
        File ser(app->GetContext(), "converted.xml", FileMode::FILE_WRITE);
        scene->SaveXML(ser);
        FinalizeScene();
    }

    // Find players
    scene->GetChildrenWithComponent<Player>(nodes, true);
    for (auto node : nodes) {
        players.push_back(node->GetComponent<Player>());
    }

    // Boundaries
    scene->GetChildrenWithComponent<RoomBoundary>(nodes, true);
    for (auto node : nodes) {
        rooms.push_back(node->GetComponent<RoomBoundary>());
    }
    scene->GetChildrenWithComponent<HouseBoundary>(nodes, true);
    URHO3D_ASSERT(!nodes.empty(), "No house boundary could be found");
    house = nodes[0]->GetComponent<HouseBoundary>();
    nodes.clear();

    // Find ghost and configure ghost if found
    scene->GetChildrenWithComponent<Ghost>(nodes, true);
    if (!nodes.empty()) {
        // Get ghost
        ghost = nodes[0]->GetComponent<Ghost>();
        nodes.clear();

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

RoomBoundary *LevelManager::getNodeRoom(Node *node) {
    auto lsize = rooms.size();
    for (auto room : rooms) {
        if (room->isNodeInside(node)) {
            return room;
        }
    }
    return nullptr;
}
}
