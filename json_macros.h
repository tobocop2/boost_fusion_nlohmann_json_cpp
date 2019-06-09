#pragma once

#include <memory>
#include <sstream>
#include <string>

#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/mpl/range_c.hpp>

#include <nlohmann/json.hpp>

/*
 * This macro provides to and from json implementations for most common types
 * The macro must be used inside of the namespace for the structure
 */
#define GENERATE_TO_AND_FROM_JSON()                                                                                          \
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
                try                                                                                                          \
                {                                                                                                            \
                    j[boost::fusion::extension::struct_member_name<T,index>::call()] =                                       \
                        boost::fusion::at_c<index>(t);                                                                       \
                }                                                                                                            \
                catch(std::exception &exc)                                                                                   \
                {                                                                                                            \
                    errors << exc.what() << std::endl;                                                                       \
                }                                                                                                            \
            }                                                                                                                \
                                                                                                                             \
        );                                                                                                                   \
                                                                                                                             \
        if (!errors.str().empty())                                                                                           \
        {                                                                                                                    \
            throw std::runtime_error(errors.str());                                                                          \
        }                                                                                                                    \
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
                    errors << exc.what() << std::endl;                                                                       \
                }                                                                                                            \
            }                                                                                                                \
        );                                                                                                                   \
                                                                                                                             \
        if (!errors.str().empty())                                                                                           \
        {                                                                                                                    \
            throw std::runtime_error(errors.str());                                                                          \
        }                                                                                                                    \
    }
