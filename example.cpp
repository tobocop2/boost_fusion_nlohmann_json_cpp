#include "json_macros.h"
#include <tuple>
#include <map>
#include <list>
#include <iostream>

using nlohmann::json;

enum Meh
{
    EH,
    FEH,
    GEH,
};

// map Meh values to JSON as strings
NLOHMANN_JSON_SERIALIZE_ENUM( Meh, {
    {EH, "EH"},
    {FEH, "FEH"},
    {GEH, "GEH"},
})

struct C
{
    int bleh;
    std::string breh;
    Meh meh;
};

BOOST_FUSION_ADAPT_STRUCT(
    C,
    bleh,
    breh,
    meh,
)

struct B
{
    int a;
    double b;
    std::list<C> c;
};

BOOST_FUSION_ADAPT_STRUCT(
    B,
    a,
    b,
    c
)

struct A {
    int x;
    float y;
    std::map<int, B> z_int_map;
    std::tuple<std::string, B, C> z_tuple;
};

BOOST_FUSION_ADAPT_STRUCT(
    A,
    x,
    y,
    z_int_map,
    z_tuple,
)

GENERATE_TO_AND_FROM_JSON()

int main()
{
    // test smallest inner structure with an enumeration
    C c { 1, "brehem", Meh::GEH };
    json CToJson = c;
    C CfromJson = CToJson.get<C>();
    json CToJsonAgain = CfromJson;

    // test nested structure with above structure embedded
    B b {
        666,
        666.0,
        { c, c }
    };

    json BToJson = b;
    B BfromJson = BToJson.get<B>();
    json jB = b;
    json BToJsonAgain = BfromJson;

    A a {
        1, // x
        2, //y
        {  // z_int_map
            {
                1, // int key
                b // B which contains C
            }
        },
        // z_tuple
        std::make_tuple<std::string, B, C>(
              "key1", // string key
              B(b), // B which contains C
              C(c) // c again cause why not
        ),
    };

    json AToJson = a;
    A AfromJson = AToJson.get<A>();
    json jA = a;
    json AToJsonAgain = AfromJson;
    std::cout << AToJson.dump(4) << std::endl;

    return 0;
}