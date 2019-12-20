// is_instance.h

#include <iostream>
#include <type_traits>
#include <string>

// https://stackoverflow.com/a/44013030

template <class, template <class> class>
struct is_instance : public std::false_type {};

template <class T, template <class> class U>

struct is_instance<U<T>, U> : public std::true_type {};
