#include "EMFReader.hpp"
#include "../LevelManager.hpp"



namespace Game {
void EMFReader::Start() {
    leds = GetNode()->GetChild("LEDs");
    TurnOff();
}

void EMFReader::TurnOn() {
    leds->GetChild(unsigned(0))->SetEnabled(true);
}

void EMFReader::TurnOff() {
    for (const auto led : leds->GetChildren()) {
        led->SetEnabled(false);
    }
}

void EMFReader::setLevel(uint8_t level) {
    TurnOff();
    for (level--; level < 5; level--) {
        leds->GetChild(level)->SetEnabled(true);
    }
}
}
