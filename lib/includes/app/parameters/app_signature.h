#ifndef APP_SIGNATURE_H
#define APP_SIGNATURE_H

#include <string>
#include <memory>
#include <typeinfo>
#include <vector>

#include "app_params.h"
#include "core/text/string_util.h"
#include "data/adt/hash_map_o.h"

namespace sk3l {
namespace app {


   // Encapsulates a collection of application pararmeters.
   template<typename charT = char,
            typename traits= std::char_traits<charT> >
   class basic_app_signature
   {
      private:
         using string_t    = std::basic_string<charT,traits>;
         using param_t     = basic_app_param<charT,traits>;
         using param_base_t= std::shared_ptr<param_t>;
         using param_map_t = sk3l::data::adt::open_hash_map<string_t, param_base_t>;
         using param_list_t= std::vector<param_base_t>;        

         static constexpr charT const * NAME_DELIM = "--";
         static constexpr charT const * ALIAS_DELIM = "-";

         param_map_t    allowed_params_;
         param_map_t    current_params_;
         param_list_t   required_params_;

      public:
         basic_app_signature()
            : allowed_params_(32, sk3l::data::adt::string_hash()),
              current_params_(32, sk3l::data::adt::string_hash())
         {}

         basic_app_signature(const param_list_t & plist)
            : allowed_params_(32, sk3l::data::adt::string_hash())
         {
            for (auto it = plist.cbegin(); it != plist.cend(); ++it)
            {
               const param_base_t & param = *it;
               this->allowed_params_.insert(param->get_name(), param);

               if (param->get_alias().size() > 0)
                  this->allowed_params_.insert(param->get_alias(), param);

               if (param->get_is_required())
                  this->required_params_.push_back(param);
            }
         }

         void insert(const param_base_t & p)
         {
            string_t name =
               sk3l::core::text::string_util::to_lower(p->get_name());

            this->allowed_params_.insert(name, p);
            if (p->get_alias().size() > 0)
            {
               string_t alias =
                  sk3l::core::text::string_util::to_lower(p->get_alias());
               this->allowed_params_.insert(alias, p);
           }

           if (p->get_is_required())
              this->required_params_.push_back(p);
 
         } 

         void remove(const param_t & p) {}   // TO DO

         bool validate(int argc, charT ** args)
         {
            param_base_t prev;
            for (int i = 1; i < argc; ++i) // Skip first argument, program name.
            {
               string_t arg(args[i]);

               if (!prev)
               {
                  typename param_map_t::iterator p;
                  // Attempt to locate a parameter matching the argument
                  // by name.
               
                  if(arg.substr(0,2) == string_t(NAME_DELIM))
                  {
                     size_t start = arg.find_first_not_of(NAME_DELIM);
                     string_t name = 
                        sk3l::core::text::string_util::to_lower
                        (
                           arg.substr(start)
                        );
                     
                     p = this->allowed_params_.find(name);
                     if (p == this->allowed_params_.end())
                        return false;
   
                  }
                  // Attempt to locate a parameter matching the argument
                  // by alias.
                  else if(arg.substr(0,1) == string_t(ALIAS_DELIM))
                  {
   
                     size_t start = arg.find_first_not_of(ALIAS_DELIM);
                     string_t alias = 
                        sk3l::core::text::string_util::to_lower
                        (
                           arg.substr(start)
                        );
  
                     p =  this->allowed_params_.find(alias);
                     if (p == this->allowed_params_.end())
                        return false;
   
                  }
                  else 
                  {
                     // Unknown param
                     return false;
                  }
   
                  // Verify the parameter is where it's expected to be.
                  int order = p->get_val()->get_order();
                  if (order != param_t::NO_ORDER && order != i)
                     return false;
   
                  this->current_params_.insert(p->get_val()->get_name(), p->get_val());
                  prev = p->get_val();
                  continue;
               }
               else 
               {
                  // If previous parameter didn't require an associated value,
                  // then this string is some unknown value. 
                  if (typeid(prev.get()) == typeid(sk3l::app::switch_app_param))
                     return false;

                  // Parse the value for the previously encountered parameter.
                  if (!prev->parse_value(arg))
                     return false;
               }
            }

            // Verify all required params are present           
            for 
            (
               auto it = this->required_params_.cbegin();
               it != this->required_params_.cend();
               ++it
            )
            {
               if (!this->current_params_.exists(it->get()->get_name()))
                  return false;
            } 

            // Verify all params encountered have an associated value.
            for
            (
               auto it = this->current_params_.begin();
               it != this->current_params_.end();
               ++it
            )
            {
               if (!it->get_val().get()->has_value())
                  return false;
            }          

            return true;
         }

         bool is_param_used(const string_t & name)
         {
            return this->current_params_.exists(name);
         }

   };
   
   using app_signature  = basic_app_signature<char, std::char_traits<char> >;
   using wapp_signature = basic_app_signature<wchar_t, std::char_traits<wchar_t> >; 
   using app_param      = basic_app_param<char, std::char_traits<char> >;
   using wapp_param     = basic_app_param<wchar_t, std::char_traits<wchar_t> >; 

}
}

#endif
