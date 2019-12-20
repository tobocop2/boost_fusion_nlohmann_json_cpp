// adl_serializer_optional_specialization.h

#include <experimental/optional>

namespace nlohmann
{
   template <typename T>
   struct adl_serializer<std::experimental::optional<T> >
   {
      static void to_json(json& j, const std::experimental::optional<T>& opt)
      {
         if (opt == std::experimental::nullopt)
         {
            j = nullptr;
         }
         else
         {
            j = *opt;
         }
      }

      static void from_json(const json& j, std::experimental::optional<T> & opt)
      {
         if (j.is_null())
         {
            opt = std::experimental::nullopt;
         }
         else
         {
            opt = j.get<T>();

         }
      }
   };
}
