#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <type_traits>

#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/mpl/range_c.hpp>

#include <nlohmann/json.hpp>

#include "adl_serializer_optional_specialization.h"
#include "is_instance.h"

/*
 * This macro provides to and from json implementations for most common types
 * The macro must be used inside of the namespace for the structure
 */
#define BOOST_FUSION_SEQUENCE_JSONIFY()                                                                                         \
    template <typename T>                                                                                                    \
    void to_json(nlohmann::json &j, const std::shared_ptr<T> &ptr)                                                           \
    {                                                                                                                        \
        j = *ptr;                                                                                                            \
    }                                                                                                                        \
                                                                                                                             \
    template <typename T>                                                                                                    \
    void to_json(nlohmann::json &j, const std::unique_ptr<T> &ptr)                                                           \
    {                                                                                                                        \
        j = *ptr;                                                                                                            \
    }                                                                                                                        \
                                                                                                                             \
    template <typename T,                                                                                                    \
              typename = typename std::enable_if<std::is_pointer<T>::value> >                                                \
    void to_json(nlohmann::json &j, const T* ptr)                                                                            \
    {                                                                                                                        \
        j = *ptr;                                                                                                            \
    }                                                                                                                        \
                                                                                                                             \
    template<typename T,                                                                                                     \
             typename = typename std::enable_if<                                                                             \
                 boost::fusion::traits::is_sequence<T>::value                                                                \
             >::type>                                                                                                        \
    void to_json(nlohmann::json& j, T const& t)                                                                              \
    {                                                                                                                        \
        std::stringstream errors;                                                                                            \
        boost::fusion::for_each(                                                                                             \
            boost::mpl::range_c<unsigned, 0, boost::fusion::result_of::size<T>::value>(),                                    \
            [&](auto index)                                                                                                  \
            {                                                                                                                \
                const auto key = boost::fusion::extension::struct_member_name<T,index>::call();                              \
                                                                                                                             \
                try                                                                                                          \
                {                                                                                                            \
                    j[key] = boost::fusion::at_c<index>(t);                                                                  \
                }                                                                                                            \
                catch(std::exception &exc)                                                                                   \
                {                                                                                                            \
                    errors << "Error with key: " << key << "\n" << exc.what() << std::endl;                                  \
                }                                                                                                            \
                                                                                                                             \
                /* if the member type is optional and null, then erase this key/value pair */                                \
                using member_type = typename boost::fusion::result_of::value_at<T, boost::mpl::int_<index> >::type;          \
                                                                                                                             \
                if (is_instance<member_type, std::experimental::optional>{} && j.at(key) == nullptr)                         \
                {                                                                                                            \
                   j.erase(key);                                                                                             \
                }                                                                                                            \
            }                                                                                                                \
        );                                                                                                                   \
                                                                                                                             \
        if (!errors.str().empty())                                                                                           \
        {                                                                                                                    \
            throw std::runtime_error(errors.str());                                                                          \
        }                                                                                                                    \
    }                                                                                                                        \
                                                                                                                             \
    template <typename T>                                                                                                    \
    void from_json(const nlohmann::json &j, std::shared_ptr<T> &ptr)                                                         \
    {                                                                                                                        \
        ptr = std::make_shared<T>(j.get<T>());                                                                               \
    }                                                                                                                        \
                                                                                                                             \
    template <typename T>                                                                                                    \
    void from_json(const nlohmann::json &j, std::unique_ptr<T> &ptr)                                                         \
    {                                                                                                                        \
        ptr = std::make_unique<T>(j.get<T>());                                                                               \
    }                                                                                                                        \
                                                                                                                             \
    template <typename T,                                                                                                    \
              typename = typename std::enable_if<std::is_pointer<T>::value> >                                                \
    void from_json(const nlohmann::json &j, T* &ptr)                                                                         \
    {                                                                                                                        \
       using underLyingType = typename std::remove_pointer<T>::type;                                                         \
       ptr = new underLyingType(j.get<underLyingType>());                                                                    \
    }                                                                                                                        \
                                                                                                                             \
    template<typename T,                                                                                                     \
             typename = typename std::enable_if<                                                                             \
                 boost::fusion::traits::is_sequence<T>::value                                                                \
             >::type>                                                                                                        \
    void from_json(const nlohmann::json& j, T &t)                                                                            \
    {                                                                                                                        \
        std::stringstream errors;                                                                                            \
        boost::fusion::for_each(                                                                                             \
            boost::mpl::range_c<unsigned, 0, boost::fusion::result_of::size<T>::value>(),                                    \
            [&](auto index)                                                                                                  \
            {                                                                                                                \
                using member_type = typename boost::fusion::result_of::value_at<T, boost::mpl::int_<index> >::type;          \
                try                                                                                                          \
                {                                                                                                            \
                    boost::fusion::at_c<index>(t) =                                                                          \
                        j.at(boost::fusion::extension::struct_member_name<T,index>::call()).template get<member_type>();     \
                }                                                                                                            \
                catch(std::exception &exc)                                                                                   \
                {                                                                                                            \
                    auto key = boost::fusion::extension::struct_member_name<T,index>::call();                                \
                    /* If it has the key and fails to deserialize, want to capture this error.                               \
                     * If it doesnt have the key and the property is not optional capture this error.                        \
                     * */                                                                                                    \
                    if (j.count(key) || (!j.count(key) && !is_instance<member_type, std::experimental::optional>{}))         \
                    {                                                                                                        \
                       errors << "Error with key: " << key << "\n" << exc.what() << std::endl;                               \
                    }                                                                                                        \
                }                                                                                                            \
            }                                                                                                                \
        );                                                                                                                   \
                                                                                                                             \
        if (!errors.str().empty())                                                                                           \
        {                                                                                                                    \
            throw std::runtime_error(errors.str());                                                                          \
        }                                                                                                                    \
    }
