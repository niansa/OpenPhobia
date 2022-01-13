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
class LevelManager : public SceneManager {
    eastl::string level = "OldHouse";

public:
    using SceneManager::SceneManager;

    void reloadLevel() {
        loadScene("Scenes/"+level+".xml");
        // Apply graphics settings
        auto renderPipeline = scene->GetOrCreateComponent<RenderPipeline>();
        auto renderSettings = renderPipeline->GetSettings();
        renderSettings.bloom_.enabled_ = true;
        renderSettings.sceneProcessor_.lightingMode_ = DirectLightingMode::DeferredBlinnPhong;
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
    }
};
}
#endif // LEVELMANAGER_HPP
