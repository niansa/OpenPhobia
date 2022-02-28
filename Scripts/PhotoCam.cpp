#include "PhotoCam.hpp"
#include "../LevelManager.hpp"
#include "EMFEmitter.hpp"



namespace Game {
void PhotoCam::TurnOn() {
    auto emitter = GetNode()->CreateComponent<EMFEmitter>();
    emitter->setLevel(EMFLevel::touch);
    turnedOn = true;
}
void PhotoCam::TurnOff() {
    GetNode()->RemoveComponent<EMFEmitter>();
    turnedOn = false;
}
}
