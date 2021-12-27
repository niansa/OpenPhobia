namespace Scatter {
class SceneManager;
}
#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP
#include "Namespace.hpp"
#include "FriendlyApp.hpp"
#include "Script.hpp"
#include "Scripts/Player.hpp"

#include <exception>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>



namespace Scatter {
struct SceneManager {
    FriendlyApp *app;
    Urho3D::Scene *scene = nullptr;
    Urho3D::Node *level;
    eastl::shared_ptr<Player> player;
    eastl::vector<eastl::shared_ptr<Script>> scripts;

    SceneManager(FriendlyApp *app) : app(app) {}

    Urho3D::Node *loadNode(const eastl::string& name);
    void loadScene(const eastl::string& filename);

    template<class ScriptT>
    eastl::shared_ptr<ScriptT> loadObject(const eastl::string& filename, const eastl::string& name) {
        auto fres = eastl::make_shared<ScriptT>();
        fres->app = app;
        fres->node = loadNode(filename)->GetChild(name);
        scripts.push_back(fres);
        fres->Start();
        return fres;
    }

    void Update();
};
}
#endif // SCENEMANAGER_HPP
