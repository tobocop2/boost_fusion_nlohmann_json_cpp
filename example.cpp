#include "json_macros.h"
#include <tuple>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <list>
#include <forward_list>
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

    bool operator==(const C &rhs) const
    {
      return this->bleh == rhs.bleh && this->breh == rhs.breh && this->meh == rhs.meh;
    }

    bool operator!=(const C &rhs) const
    {
      return !(*this == rhs);
    }

    bool operator<(const C &rhs) const
    {
      return this->bleh < rhs.bleh;
    }

    bool operator>=(const C &rhs) const
    {
      return !(*this < rhs);
    }
};

namespace std
{
    template <>
    struct hash<C>
    {
        size_t operator()(const C& c) const
        {
            // Compute individual hash values for two data members and combine them using XOR and bit shifting
            return ((hash<int>()(c.bleh) ^ (hash<std::string>()(c.breh )) ^ (hash<Meh>()(c.meh) >> 1)));
        }
    };
}

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

struct D {
    std::set<C> c_set;
    std::unordered_set<C> c_unordered_set;
    std::unordered_map<int, C> c_unordered_map;
    std::array<B, 2> b_arr;
    std::forward_list<A> a_flist;
};

BOOST_FUSION_ADAPT_STRUCT(
    D,
    c_set,
    c_unordered_set,
    c_unordered_map,
    b_arr,
    a_flist,
)

FUSION_JSONIFY()

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
    json AToJsonAgain = AfromJson;

    D d {
      {c, c},
      {c, c},
      { {1, c } },
      {b, b},
      {a, a, a}
    };

    json DToJson = d;
    D DfromJson = DToJson.get<D>();
    json DToJsonDagain = DfromJson;
    std::cout << DToJson.dump(4) << std::endl;

    return 0;
}
