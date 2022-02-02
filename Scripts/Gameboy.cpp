#include "Gameboy.hpp"
#include "../LevelManager.hpp"
#include "EMFEmitter.hpp"



namespace Game {
void Gameboy::TurnOn() {
    auto emitter = GetNode()->CreateComponent<EMFEmitter>();
    emitter->setLevel(EMFLevel::touch);
    turnedOn = true;
}
void Gameboy::TurnOff() {
    GetNode()->RemoveComponent<EMFEmitter>();
    turnedOn = false;
}
}
