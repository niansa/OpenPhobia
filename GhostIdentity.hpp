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
struct GhostIdentity {
    const static eastl::vector<eastl::string_view> firstNamesMale,
                                                   firstNamesFemale,
                                                   lastNames;

    eastl::string_view firstName, lastName;
    unsigned age;
    enum Gender : bool {male = 0, female = 1} gender;

    GhostIdentity() {}
    GhostIdentity(unsigned seed);

    eastl::string getFullName() const {
        return eastl::string(firstName) + ' ' + eastl::string(lastName);
    }
};
}
#endif // GHOSTIDENTITY_HPP
