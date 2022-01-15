#include "EMFEmitter.hpp"



namespace Game {
void EMFEmitter::FixedUpdate(float) {
    if (timeout) {
        if (timer.GetMSec(false) > timeout) {
            Remove();
        }
    }
}
}
