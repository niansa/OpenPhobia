namespace Scatter {
class LevelManager;
}
#ifndef LEVELMANAGER_HPP
#define LEVELMANAGER_HPP
#include "easyscript/Namespace.hpp"
#include "easyscript/SceneManager.hpp"

#include <Urho3D/Engine/Application.h>



namespace Game {
class LevelManager : public SceneManager {
    eastl::string level = "OldHouse";

public:
    using SceneManager::SceneManager;

    void reloadLevel() {
        loadScene("Scenes/"+level+".xml");
    }
    void loadLevel(eastl::string nlevel) {
        level = nlevel;
        reloadLevel();
    }

    void Start() {
        reloadLevel();
    }
};
}
#endif // LEVELMANAGER_HPP
