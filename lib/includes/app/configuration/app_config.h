#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <exception>

#include "core/text/string_util.h"
#include "data/xchange/xml/xml_document.h"

namespace sk3l {

namespace app {

class app_config
{
private:
   const std::string & fileName_;
   
   using entry_t = sk3l::data::xchange::xml_element;
   using entry_it = typename entry_t::iterator;
   using config_t = sk3l::data::xchange::xml_document;
   using string_util = sk3l::core::text::string_util;
      
   config_t conf_file_;

public:

   explicit app_config(const std::string & fileName);

   template<typename T>
   T get(const std::string & path)
   {
      entry_it eit = this->conf_file_.at(path);
      if (eit == entry_it(nullptr))
         throw std::invalid_argument
         (
            std::string("Couldn't locate config entry at path '") +
            path + "'"
         );

      T rv = string_util::parse<T>(eit->get_value()); 

      return rv;
  }

   template<typename T>
   T get_or_default(const std::string & path, const T & defaultValue)
   {
      T rv;
      entry_it eit = this->conf_file_.at(path);
      if (eit == entry_it(nullptr))
         rv = defaultValue;
      else
         sk3l::core::text::string_util::parse<T>(eit->get_value(), rv);  

      return rv;
   }

   template<typename T>
   bool try_get(const std::string & path, T & t)
   {
      bool rv = false;

      entry_it eit = this->conf_file_.at(path);
      if (eit != entry_it(nullptr))
      {
         sk3l::core::text::string_util::parse<T>(eit->get_value(), t); 
         rv = true;
      }
      return rv;
   }
};

}
}

#endif // APP_CONFIG_H
