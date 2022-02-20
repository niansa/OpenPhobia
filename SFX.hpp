#pragma once

#ifndef SFX_HPP
#define SFX_HPP
#include "easyscript/Namespace.hpp"
#include "GhostIdentity.hpp"

#include <EASTL/string.h>
#include <Urho3D/Math/RandomEngine.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Audio/Sound.h>



namespace Game {
Sound *getGhostSFX(Context *ctx, RandomEngine& rng, const eastl::string& kind, GhostIdentity::Gender gender);
}
#endif
