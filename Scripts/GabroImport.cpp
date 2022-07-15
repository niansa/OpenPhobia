#include "GabroImport.hpp"
#include "Door.hpp"
#include "Lightswitch.hpp"
#include "Lightbulb.hpp"
#include "EMFEmitter.hpp"

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/PrefabReference.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Navigation/DynamicNavigationMesh.h>



namespace Game {
void GabroImport::Start() {
    // Get scene
    auto scene = GetScene()->GetChild("SceneMain");
    // Inline house prefab
    scene->GetChild("House")->GetComponent<PrefabReference>()->Inline();
    // Create rooms
    for (const auto& rProb : scene->GetChild("Rooms")->GetChildren()) {
        rProb->CreateComponent("RoomBoundary");
    }
    // Create doors node
    auto doors = scene->CreateChild("Doors");
    // Iterate over entire scene
    eastl::vector<Node*> lamps, lightswitches;
    for (auto& node : scene->GetChildren(true)) {
        // Adjust zones
        if (node->HasComponent<Zone>()) {
            auto zone = node->GetComponent<Zone>();
            zone->SetBackgroundBrightness(0.0f);
            zone->SetAmbientBrightness(0.0f);
            zone->SetZoneTexture(nullptr);
            zone->SetFogEnd(250.0f);
            zone->SetFogColor(zone->GetAmbientColor());
        }
        // Create doors
        if (node->GetName().starts_with("Door_") && !node->GetName().ends_with("_Handle")
                && node->HasComponent("RigidBody") && !node->HasComponent<Door>()) {
            auto transform = node->GetWorldTransform();
            doors->AddChild(node);
            node->SetWorldTransform(transform);
            node->CreateComponent<Door>();
        }
        // Create lightbulbs and register lamps
        else if (node->GetName() == "Point%20light" && node->GetParent()->GetName().starts_with("Lamp_")) {
            node->GetOrCreateComponent<Lightbulb>();
            lamps.push_back(node);
        }
        // Other stuff
        else if (node->GetName().starts_with("Lightswitch_") && !node->GetName().ends_with("_Knob")) {
            node->GetOrCreateComponent<Lightswitch>();
            node->SetVar("Object", "Lightswitch");
            node->AddTag("Useable");
            lightswitches.push_back(node);
        }
    }
    // Connect lightswitches to lamps
    for (Node *lamp : lamps) {
        auto bulb = lamp->GetComponent<Lightbulb>();
        Variant lampID;
        while (lamp) {
            lampID = lamp->GetVar("LampID");
            if (lampID.IsEmpty()) {
                lamp = lamp->GetParent();
            } else {
                break;
            }
        }
        if (!lampID.IsEmpty()) {
            for (Node *lightswitch : lightswitches) {
                Variant lightswitchID;
                while (lightswitch) {
                    lightswitchID = lightswitch->GetVar("LampID");
                    if (lightswitchID.IsEmpty()) {
                        lightswitch = lightswitch->GetParent();
                    } else {
                        break;
                    }
                }
                if (!lightswitchID.IsEmpty()) {
                    if (lampID.GetInt() == lightswitchID.GetInt()) {
                        lightswitch->GetComponent<Lightswitch>()->addLightbulb(bulb);
                    }
                }
            }
        }
    }
    // Configure lamps
    //TODO: Create interactables
    // Rebuild navigation mesh
    GetScene()->GetChild("SceneMain")->GetComponent<DynamicNavigationMesh>()->Build();
}
}
