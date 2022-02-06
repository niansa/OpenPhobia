#include "GhostRoam.hpp"
#include "Ghost.hpp"



namespace Game {
void GhostRoam::Initialize() {
    GetGhost()->setNextState("Local", GetGhost()->rng.GetUInt(5000, Max(20000.0f*GetGhost()->getAggression(), 5000)));
}

void GhostRoam::Deinitialize() {
    GetGhost()->walkTo(GetGhost()->homePosition);
}

void GhostRoam::FixedUpdate(float) {
    GetGhost()->roam();
}
}
