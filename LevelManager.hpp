#pragma once

namespace Scatter {
class LevelManager;
}
#ifndef LEVELMANAGER_HPP
#define LEVELMANAGER_HPP
#include "easyscript/Namespace.hpp"
#include "easyscript/SceneManager.hpp"

#include <Urho3D/Engine/Application.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/RenderPipeline/RenderPipeline.h>
#include <Urho3D/RenderPipeline/BloomPass.h>
#include <Urho3D/RenderPipeline/SceneProcessor.h>



namespace Game {
class LevelManager : public SceneManager {
    eastl::string level = "testmap";

public:
    using SceneManager::SceneManager;

    void reloadLevel() {
        loadScene("Scenes/"+level+".xml");
        // Apply graphics settings
        auto renderPipeline = scene->GetOrCreateComponent<RenderPipeline>();
        auto renderSettings = renderPipeline->GetSettings();
        renderSettings.bloom_.enabled_ = false;
        renderSettings.bloom_.intensity_ = 1.0f;
        renderSettings.sceneProcessor_.lightingMode_ = DirectLightingMode::Forward;
        renderSettings.sceneProcessor_.maxPixelLights_ = 10;
        renderSettings.renderBufferManager_.colorSpace_ = RenderPipelineColorSpace::LinearLDR;
        renderSettings.AdjustToSupported(app->GetContext());
        renderPipeline->SetSettings(renderSettings);
    }
    void loadLevel(eastl::string nlevel) {
        level = nlevel;
        reloadLevel();
    }

    void Start() {
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
    }

    unsigned getTeamSanity() const {
        return 5;
    }
};
}
#endif // LEVELMANAGER_HPP
