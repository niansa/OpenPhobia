namespace Scatter {
class Script;
}
#ifndef OBJECT_HPP
#define OBJECT_HPP
#include "Namespace.hpp"
#include "FriendlyApp.hpp"

#include <Urho3D/Scene/Node.h>



namespace Scatter {
class Script : public eastl::enable_shared_from_this<Script> {
    friend SceneManager;

protected:
    Node *node;
    FriendlyApp *app;

public:
    virtual void Start() {}
    virtual bool Update() {return true;}
    virtual ~Script() {}
};
}
#endif // OBJECT_HPP
