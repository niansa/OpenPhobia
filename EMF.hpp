#pragma once
#include "easyscript/Namespace.hpp"



namespace Game {
enum class EMFLevel {
    basic = 1,
    touch = 2,
    throw_ = 3,
    reveal = 4,
    five = 5
};

constexpr float defaultEmfTimeout = 20000.0f;
}
