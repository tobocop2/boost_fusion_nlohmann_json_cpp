# README

This library defines a bunch of generic to_json and from_json functions to easily serialize and deserialize types in any namespace

It depends on nlohmann/json and a few boost headers. 

To build the example, you either need a full boost installation available or you can clone this repo recursively and invoke the top level Makefile

Why did I do this? Serialization and deserialization are extremely cumbersome in c++ so I made this utility for myself to easily go to and from
json in the most convenient way possible. The only structure that currently maps to json that I have not supported is a C style array, but most modern
programs likely use STL containers. 

Performance is not a concern for my use cases and I don't think I have seen any examples
that are nearly as simple and DRY as this


# Example 
```c++

#include <tuple>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <list>
#include <forward_list>
#include <memory>
#include <iostream>

#include "boost_fusion_sequence_jsonify.h"

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

struct E {
    C *c_raw_ptr;
    std::shared_ptr<C>  c_shared_ptr;
    std::unique_ptr<C>  c_unique_ptr;
};

BOOST_FUSION_ADAPT_STRUCT(
    E,
    c_raw_ptr,
    c_shared_ptr,
    c_unique_ptr
)

BOOST_FUSION_SEQUENCE_JSONIFY()

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

    E e {
      new C(c),
      std::make_shared<C>(c),
      std::make_unique<C>(c)
    };

    json EToJson = e;
    E EfromJson = EToJson.get<E>();
    json EToJsonEagain = EfromJson;
    std::cout << EToJson.dump(4) << std::endl;

    delete e.c_raw_ptr;
    delete EfromJson.c_raw_ptr;

    return 0;
}
```

# Supported container types 

## Sequence types supported:
* array
* vector
* deque
* list
* forward_list

## Ordered Associative containers Supported :
* map **Key must be integral or string**
* set **Key must have comparison operator or will not compile**

## Unordered associative containers Supported :
* unordered_map **Key must be integral or string **
* unordered_set **Key MUST be hashable or will not compile **

## Heterogeneous types supported:
* pair
* tuple 

## Smart pointers supported 
* unique_ptr
* shared_ptr

# Unsupported Types with no plans to be supported

## Following Smart pointer types not supported
* weak_ptr // requires a smart pointer to be instantiated 
* auto_ptr // will be deprecated so no plan for this

## Primitive pointer / array types not supported
* C arrays (TODO)


## Container adaptors not yet supported and not planned to be supported:
* stack
* queue
* priority_queue

## Ordered Associative containers not planned to be supported  
* multiset ** no translation to json **

## Unordered Associative containers not supported and not planned to be supported:
* unordered_multiset ** no translation to json **
* unordered_multimap ** no translation to json **
