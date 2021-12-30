namespace Scatter {
class LevelManager;
}
#ifndef LEVELMANAGER_HPP
#define LEVELMANAGER_HPP
#include "easyscript/Namespace.hpp"
#include "easyscript/SceneManager.hpp"



namespace Game {
class LevelManager : public SceneManager {
    int level = 1;

public:
    using SceneManager::SceneManager;

    void reloadLevel() {
        loadScene("Scenes/level"+eastl::to_string(level)+".xml");
        player->setLevelManager(this);
    }
    void loadLevel(int nlevel) {
        level = nlevel;
        reloadLevel();
    }
    void loadNextLevel() {
        level++;
        reloadLevel();
    }

    void Start() {
        reloadLevel();
    }
};
}
#endif // LEVELMANAGER_HPP
