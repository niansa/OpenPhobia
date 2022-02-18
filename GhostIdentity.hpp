#pragma once

namespace Game {
struct GhostIdentity;
}
#ifndef GHOSTIDENTITY_HPP
#define GHOSTIDENTITY_HPP
#include "easyscript/Namespace.hpp"

#include <EASTL/string.h>
#include <EASTL/string_view.h>
#include <EASTL/vector.h>
#include <cstdint>



namespace Game {
enum class GhostType {
    Spirit,
    Wraith,
    Phantom,
    Poltergeist,
    Banshee,
    Jinn,
    Mare,
    Revenant,
    Shade,
    Demon,
    Yurei,
    Oni,
    Yokai,
    Hantu,
    Goryo,
    Myling,
    Onryo,
    Twins,
    Raiju,
    Obake,
    Mimic,
    _lowest = Spirit,
    _highest = Mimic
};

struct GhostIdentity {
    const static eastl::vector<eastl::string_view> firstNamesMale,
                                                   firstNamesFemale,
                                                   lastNames;

    eastl::string_view firstName, lastName;
    unsigned age;
    enum Gender : bool {male = 0, female = 1} gender;
    GhostType type;
    float agression;

    GhostIdentity() {}
    GhostIdentity(unsigned seed);

    eastl::string getFullName() const {
        return eastl::string(firstName) + ' ' + eastl::string(lastName);
    }
};
}
#endif // GHOSTIDENTITY_HPP
