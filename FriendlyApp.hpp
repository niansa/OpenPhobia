#ifndef FRIENDLYAPP_HPP
#define FRIENDLYAPP_HPP
#include "Namespace.hpp"

#include <Urho3D/Engine/Application.h>



namespace Scatter {
class SceneManager;

class FriendlyApp : public Application {
    friend SceneManager;

public:
    using Application::Application;
};
}
#endif // FRIENDLYAPP_HPP
