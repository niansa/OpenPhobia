#pragma once
#include "easyscript/Namespace.hpp"

#include <cstdint>



namespace Game {
namespace Evidence {
using Type = uint8_t;
enum : Type {
    EMFLevelFive =   0b00000001,
    DOTSProjection = 0b00000010,
    Fingerprints =   0b00000100,
    GhostOrbs =      0b00001000,
    GhostWriting =   0b00010000,
    SpiritBox =      0b00100000,
    FreezingTemps =  0b01000000
};

inline bool hasEvidence(Type list, Type evidence) {
    return list & evidence;
}
}
}
