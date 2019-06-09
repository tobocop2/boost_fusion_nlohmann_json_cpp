#pragma once

#include <memory>
#include <utility>
#include <tuple>
#include <sstream>
#include <string>

#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/mpl/range_c.hpp>

#include <nlohmann/json.hpp>

/* NOTE: This macro is available as of nlohmann/json 3.4.0
 * This code has been adapted from the following github issue:
 * https://github.com/nlohmann/json/issues/1208
 * It has been updated to use a C style array in place of vector so it can
 * be used without any stl includes
 *
 * It must be used within the namespace of the enumeration passed in
 */
#define NLOHMANN_JSON_SERIALIZE_ENUM( ENUM_TYPE, ...)                                  \
    inline void to_json( nlohmann::json& j, const ENUM_TYPE& e)                        \
    {                                                                                  \
        static_assert(std::is_enum<ENUM_TYPE>::value, #ENUM_TYPE " must be an enum!"); \
        static const std::pair<ENUM_TYPE, nlohmann::json> m[] = __VA_ARGS__;           \
        auto it = std::find_if(std::begin(m),                                          \
                               std::end(m),                                            \
                               [e](const auto& ej_pair)->bool                          \
        {                                                                              \
            return ej_pair.first == e;                                                 \
        });                                                                            \
        j = ((it != std::end(m)) ? it : std::begin(m))->second;                        \
    }                                                                                  \
                                                                                       \
    inline void from_json( const nlohmann::json& j, ENUM_TYPE& e)                      \
    {                                                                                  \
        static_assert(std::is_enum<ENUM_TYPE>::value, #ENUM_TYPE " must be an enum!"); \
        static const std::pair<ENUM_TYPE, nlohmann::json> m[] = __VA_ARGS__;           \
        auto it = std::find_if(std::begin(m),                                          \
                               std::end(m),                                            \
                               [j](const auto& ej_pair)->bool                          \
        {                                                                              \
            return ej_pair.second == j;                                                \
        });                                                                            \
        e = ((it != std::end(m)) ? it : std::begin(m))->first;                         \
    }

/*
 * This macro provides to and from json implementations for most common types
 * The macro must be used inside of the namespace for the structure
 */
#define GENERATE_TO_AND_FROM_JSON()                                                                                        \
    template <template<typename, typename, typename, typename> class C,                                                    \
              template <typename> class Comp,                                                                              \
              template <typename> class A, typename K, typename T,                                                         \
              typename = typename std::enable_if<                                                                          \
                  std::is_integral<K>::value ||                                                                            \
                  std::is_same<K, std::string>::value>::type>                                                              \
    void to_json(nlohmann::json &j, const C<K, T, Comp<K>, A<std::pair<const K,T> > > &mapIn)                              \
    {                                                                                                                      \
        std::stringstream errors;                                                                                          \
        for (const auto& kv: mapIn)                                                                                        \
        {                                                                                                                  \
            try                                                                                                            \
            {                                                                                                              \
                nlohmann::json j_val = kv.second;                                                                          \
                j[boost::lexical_cast<std::string>(kv.first)] = j_val;                                                     \
            }                                                                                                              \
            catch(std::exception &exc)                                                                                     \
            {                                                                                                              \
                errors << exc.what() << std::endl;                                                                         \
            }                                                                                                              \
        }                                                                                                                  \
                                                                                                                           \
        if (!errors.str().empty())                                                                                         \
        {                                                                                                                  \
            std::runtime_error(errors.str());                                                                              \
        }                                                                                                                  \
    }                                                                                                                      \
                                                                                                                           \
    template <template <typename...> class SequenceType, typename T>                                                       \
    void to_json(nlohmann::json &j, const SequenceType<T> &objs)                                                           \
    {                                                                                                                      \
        std::stringstream errors;                                                                                          \
        for (const auto& object : objs)                                                                                    \
        {                                                                                                                  \
            try                                                                                                            \
            {                                                                                                              \
                j.emplace_back(object);                                                                                    \
            }                                                                                                              \
            catch(std::exception &exc)                                                                                     \
            {                                                                                                              \
                errors << exc.what() << std::endl;                                                                         \
            }                                                                                                              \
        }                                                                                                                  \
                                                                                                                           \
        if (!errors.str().empty())                                                                                         \
        {                                                                                                                  \
            std::runtime_error(errors.str());                                                                              \
        }                                                                                                                  \
    }                                                                                                                      \
                                                                                                                           \
    template <typename... Args>                                                                                            \
    void to_json(nlohmann::json &j, const std::pair<Args...>& p)                                                           \
    {                                                                                                                      \
        j = {p.first, p.second};                                                                                           \
    }                                                                                                                      \
                                                                                                                           \
                                                                                                                           \
    template <typename Tuple, std::size_t... Idx>                                                                          \
    void to_json_tuple_impl(nlohmann::json &j, const Tuple& t, nlohmann::detail::index_sequence<Idx...>)                   \
    {                                                                                                                      \
        j = {std::get<Idx>(t)...};                                                                                         \
    }                                                                                                                      \
                                                                                                                           \
    template <typename... Args>                                                                                            \
    void to_json(nlohmann::json &j, const std::tuple<Args...>& t)                                                          \
    {                                                                                                                      \
        to_json_tuple_impl(j, t, nlohmann::detail::index_sequence_for<Args...> {});                                        \
    }                                                                                                                      \
                                                                                                                           \
    template <typename T>                                                                                                  \
    void to_json(nlohmann::json &j, const std::shared_ptr<T> &ptr)                                                         \
    {                                                                                                                      \
        j = *ptr;                                                                                                          \
    }                                                                                                                      \
                                                                                                                           \
    template <typename T>                                                                                                  \
    void to_json(nlohmann::json &j, const std::unique_ptr<T> &ptr)                                                         \
    {                                                                                                                      \
        j = *ptr;                                                                                                          \
    }                                                                                                                      \
                                                                                                                           \
    template<typename T,                                                                                                   \
             typename = typename std::enable_if<                                                                           \
                 boost::fusion::traits::is_sequence<T>::value                                                              \
             >::type>                                                                                                      \
    void to_json(nlohmann::json& j, T const& t)                                                                            \
    {                                                                                                                      \
        std::stringstream errors;                                                                                          \
        boost::fusion::for_each(                                                                                           \
            boost::mpl::range_c<unsigned, 0, boost::fusion::result_of::size<T>::value>(),                                  \
            [&](auto index)                                                                                                \
            {                                                                                                              \
                try                                                                                                        \
                {                                                                                                          \
                    j[boost::fusion::extension::struct_member_name<T,index>::call()] =                                     \
                        boost::fusion::at_c<index>(t);                                                                     \
                }                                                                                                          \
                catch(std::exception &exc)                                                                                 \
                {                                                                                                          \
                    errors << exc.what() << std::endl;                                                                     \
                }                                                                                                          \
            }                                                                                                              \
                                                                                                                           \
        );                                                                                                                 \
                                                                                                                           \
        if (!errors.str().empty())                                                                                         \
        {                                                                                                                  \
            std::runtime_error(errors.str());                                                                              \
        }                                                                                                                  \
    }                                                                                                                      \
                                                                                                                           \
    template <template<typename, typename, typename, typename> class C,                                                    \
              template <typename> class Comp,                                                                              \
              template <typename> class A, typename K, typename T,                                                         \
              typename = typename std::enable_if<                                                                          \
                  std::is_integral<K>::value ||                                                                            \
                  std::is_same<K, std::string>::value>::type>                                                              \
    void from_json(const nlohmann::json &j, C<K, T, Comp<K>, A<std::pair<const K,T> > > &mapIn)                            \
    {                                                                                                                      \
        std::stringstream errors;                                                                                          \
        for (const auto& it : j.items())                                                                                   \
        {                                                                                                                  \
            try                                                                                                            \
            {                                                                                                              \
                mapIn.emplace(boost::lexical_cast<K>(it.key()), it.value());                                               \
            }                                                                                                              \
            catch(std::exception &exc)                                                                                     \
            {                                                                                                              \
                errors << exc.what() << std::endl;                                                                         \
            }                                                                                                              \
        }                                                                                                                  \
                                                                                                                           \
        if (!errors.str().empty())                                                                                         \
        {                                                                                                                  \
            std::runtime_error(errors.str());                                                                              \
        }                                                                                                                  \
    }                                                                                                                      \
                                                                                                                           \
    template <template <typename...> class SequenceType, typename T>                                                       \
    void from_json(const nlohmann::json &j, SequenceType<T> &objs)                                                         \
    {                                                                                                                      \
        std::stringstream errors;                                                                                          \
        for (const auto& val: j)                                                                                           \
        {                                                                                                                  \
            try                                                                                                            \
            {                                                                                                              \
                objs.emplace_back(val.get<T>());                                                                           \
            }                                                                                                              \
            catch(std::exception &exc)                                                                                     \
            {                                                                                                              \
                errors << exc.what() << std::endl;                                                                         \
            }                                                                                                              \
        }                                                                                                                  \
                                                                                                                           \
        if (!errors.str().empty())                                                                                         \
        {                                                                                                                  \
            std::runtime_error(errors.str());                                                                              \
        }                                                                                                                  \
    }                                                                                                                      \
                                                                                                                           \
    template <typename T>                                                                                                  \
    void from_json(const nlohmann::json &j, std::forward_list<T> &objs)                                                    \
    {                                                                                                                      \
        std::stringstream errors;                                                                                          \
        for (const auto& val: j)                                                                                           \
        {                                                                                                                  \
            try                                                                                                            \
            {                                                                                                              \
                objs.emplace_front(val.get<T>());                                                                          \
            }                                                                                                              \
            catch(std::exception &exc)                                                                                     \
            {                                                                                                              \
                errors << exc.what() << std::endl;                                                                         \
            }                                                                                                              \
        }                                                                                                                  \
                                                                                                                           \
        if (!errors.str().empty())                                                                                         \
        {                                                                                                                  \
            throw std::runtime_error(errors.str());                                                                        \
        }                                                                                                                  \
    }                                                                                                                      \
                                                                                                                           \
    template <typename T>                                                                                                  \
    void from_json(const nlohmann::json &j, std::set<T> &objs)                                                             \
    {                                                                                                                      \
        std::stringstream errors;                                                                                          \
        for (const auto& val: j)                                                                                           \
        {                                                                                                                  \
            try                                                                                                            \
            {                                                                                                              \
                objs.emplace(val.get<T>());                                                                                \
            }                                                                                                              \
            catch(std::exception &exc)                                                                                     \
            {                                                                                                              \
                errors << exc.what() << std::endl;                                                                         \
            }                                                                                                              \
        }                                                                                                                  \
                                                                                                                           \
        if (!errors.str().empty())                                                                                         \
        {                                                                                                                  \
            throw std::runtime_error(errors.str());                                                                        \
        }                                                                                                                  \
    }                                                                                                                      \
                                                                                                                           \
    template <typename T, size_t N>                                                                                        \
    void from_json(const nlohmann::json &j, std::array<T, N> &objs)                                                        \
    {                                                                                                                      \
        std::stringstream errors;                                                                                          \
        if (j.size() != objs.size())                                                                                       \
        {                                                                                                                  \
            errors << "Out bounds. json_array size: " << j.size() << "\ninput array size: " << objs.size() << " \n";       \
        }                                                                                                                  \
        else                                                                                                               \
        {                                                                                                                  \
          for (size_t i = 0; i < j.size(); ++i)                                                                            \
          {                                                                                                                \
              try                                                                                                          \
              {                                                                                                            \
                  objs[i] = j.at(i).get<T>();                                                                              \
              }                                                                                                            \
              catch(std::exception &exc)                                                                                   \
              {                                                                                                            \
                  errors << exc.what() << std::endl;                                                                       \
              }                                                                                                            \
          }                                                                                                                \
        }                                                                                                                  \
                                                                                                                           \
        if (!errors.str().empty())                                                                                         \
        {                                                                                                                  \
            std::runtime_error(errors.str());                                                                              \
        }                                                                                                                  \
    }                                                                                                                      \
                                                                                                                           \
    template <typename T>                                                                                                  \
    void from_json(const nlohmann::json &j, std::shared_ptr<T> &ptr)                                                       \
    {                                                                                                                      \
        ptr = std::make_shared<T>(j.get<T>());                                                                             \
    }                                                                                                                      \
                                                                                                                           \
    template <typename T>                                                                                                  \
    void from_json(const nlohmann::json &j, std::unique_ptr<T> &ptr)                                                       \
    {                                                                                                                      \
        ptr = std::make_unique<T>(j.get<T>());                                                                             \
    }                                                                                                                      \
                                                                                                                           \
    template <typename A, typename B >                                                                                     \
    void from_json(const nlohmann::json &j, std::pair<A, B> &p)                                                            \
    {                                                                                                                      \
        p = std::make_pair(j.at(0).get<A>(), j.at(1).get<B>());                                                            \
    }                                                                                                                      \
                                                                                                                           \
    template <typename Tuple, std::size_t... Idx>                                                                          \
    void from_json_tuple_impl(const nlohmann::json &j, Tuple& t, nlohmann::detail::index_sequence<Idx...>)                 \
    {                                                                                                                      \
        t = std::make_tuple(j.at(Idx).get<typename std::tuple_element<Idx, Tuple>::type>()...);                            \
    }                                                                                                                      \
                                                                                                                           \
    template <typename... Args>                                                                                            \
    void from_json(const nlohmann::json &j, std::tuple<Args...>&t)                                                         \
    {                                                                                                                      \
        from_json_tuple_impl(j, t, nlohmann::detail::index_sequence_for<Args...> {});                                      \
    }                                                                                                                      \
                                                                                                                           \
    template<typename T,                                                                                                   \
             typename = typename std::enable_if<                                                                           \
                 boost::fusion::traits::is_sequence<T>::value                                                              \
             >::type>                                                                                                      \
    void from_json(const nlohmann::json& j, T &t)                                                                          \
    {                                                                                                                      \
        std::stringstream errors;                                                                                          \
        boost::fusion::for_each(                                                                                           \
            boost::mpl::range_c<unsigned, 0, boost::fusion::result_of::size<T>::value>(),                                  \
            [&](auto index)                                                                                                \
            {                                                                                                              \
                using member_type = typename boost::fusion::result_of::value_at<T, boost::mpl::int_<index> >::type;        \
                try                                                                                                        \
                {                                                                                                          \
                    boost::fusion::at_c<index>(t) =                                                                        \
                        j.at(boost::fusion::extension::struct_member_name<T,index>::call()).template get<member_type>();   \
                }                                                                                                          \
                catch(std::exception &exc)                                                                                 \
                {                                                                                                          \
                    errors << exc.what() << std::endl;                                                                     \
                }                                                                                                          \
            }                                                                                                              \
        );                                                                                                                 \
                                                                                                                           \
        if (!errors.str().empty())                                                                                         \
        {                                                                                                                  \
            std::runtime_error(errors.str());                                                                              \
        }                                                                                                                  \
    }
