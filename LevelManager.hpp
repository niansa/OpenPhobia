#pragma once

namespace Scatter {
class LevelManager;
}
#ifndef LEVELMANAGER_HPP
#define LEVELMANAGER_HPP
#include "easyscript/Namespace.hpp"
#include "easyscript/SceneManager.hpp"

#include <Urho3D/Engine/Application.h>
#include <Urho3D/RenderPipeline/RenderPipeline.h>
#include <Urho3D/RenderPipeline/BloomPass.h>
#include <Urho3D/RenderPipeline/SceneProcessor.h>



namespace Game {
class Ghost;

class LevelManager : public SceneManager {
    eastl::string level = "testmap";
    Ghost *ghost = nullptr;

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

    void Start();

    unsigned getTeamSanity() const {
        return 5;
    }

    Ghost *getGhost() {
        return ghost;
    }
};
}
#endif // LEVELMANAGER_HPP
