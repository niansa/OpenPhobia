#include "GhostLocal.hpp"
#include "Ghost.hpp"
#include "RoomBoundary.hpp"



namespace Game {
void GhostLocal::Initialize() {
    // Find and define next state
    eastl::string nState;
    auto teamSanity = GetGhost()->levelManager->getTeamSanity();
    if (teamSanity < GetGhost()->behavior->sanityThreshold + GetGhost()->behavior->getHuntMultiplier() && GetGhost()->lastHuntTimer.GetMSec(false) > (GetGhost()->behavior->huntCooldown - 1000) && GetGhost()->rng.GetBool(teamSanity>25?0.25f:0.5f)) {
        nState = "Hunt";
    } else if (GetGhost()->rng.GetBool(0.025f*GetGhost()->getAggression())) {
        nState = "Reveal";
    } else  {
        nState = "Roam";
    }
    GetGhost()->setNextState(nState, GetGhost()->rng.GetUInt(5000, Clamp<unsigned>(20000.0f/(GetGhost()->getAggression()*4.0f), 5000, 30000)));
}

void GhostLocal::FixedUpdate(float) {
    if (GetGhost()->currentPath.empty()) {
        auto cRoom = GetGhost()->getCurrentRoom();
        if (cRoom) {
            // Go to random location inside of the room
            GetGhost()->walkTo(GetGhost()->rng.GetVector3(cRoom->getBoundingBox()));
        }
    }
}
}
