#include "GhostIdentity.hpp"
#include "easyscript/Namespace.hpp"

#include <Urho3D/Math/RandomEngine.h>



namespace Game {
const eastl::vector<eastl::string_view> GhostIdentity::firstNamesMale = {"Jack", "Brian", "Richard", "Luke", "Russell", "Larry", "Carlos", "Robert", "Jason", "Daniel", "Donald", "Steven", "David", "Christopher", "Anthony", "James", "Mark", "Justin", "Corey", "Joseph", "Kenneth", "Charles", "Gary", "George", "John", "Benjamin", "Walter", "Kevin", "Jerry", "Billy", "Eric", "Harold", "Ronald", "William", "Pater", "Paul", "Michael", "Edward", "Thomas", "Raymond"},
                                        GhostIdentity::firstNamesFemale = {"Ruth", "Betty", "Lisa", "Georgia", "Dorothy", "Jennifer", "Karen", "Barbara", "Julie", "Ann", "Nancy", "Lucy", "Judy", "Gloria", "Nellie", "Megan", "Susan", "Mary", "Becky", "Patricia", "Jane", "Carla", "Sandra", "Marcia", "Ellen", "Linda", "Margaret", "Maria", "Helen", "Donna", "Elizabeth", "Holly", "Stacey", "Ella", "Carol"},
                                        GhostIdentity::lastNames = {"Hall", "Anderson", "Wright", "Young", "Carter", "Brown", "Taylor", "Lewis", "Smith", "Moore", "Williams", "Johnson", "Dixon", "Jones", "Jackson", "Garcia", "Roberts", "Walker", "Harris", "Martin", "Clarke", "Baker", "Miller", "Hill", "White", "Robinson", "Wilson", "Martinez", "Thompson"};

GhostIdentity::GhostIdentity(unsigned seed) {
    RandomEngine rng(seed);

    // Get random gender
    gender = static_cast<Gender>(rng.GetBool(0.5f));

    // Get name list to use
    const auto& firstNames = (gender == male)?firstNamesMale:firstNamesFemale;

    // Get random first and last name
    firstName = firstNames[rng.GetUInt(0, firstNames.size())];
    lastName = lastNames[rng.GetUInt(0, lastNames.size())];

    // Get random age
    age = rng.GetUInt(10, 90);

    // Get random type
    type = static_cast<GhostType>(rng.GetUInt(unsigned(GhostType::_lowest), unsigned(GhostType::_highest)));
}
}
