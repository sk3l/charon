#ifndef APP_PARAMS_H
#define APP_PARAMS_H

#include <string>

#include "core/nullable.h"
#include "core/text/string_util.h"

namespace sk3l {
namespace app {

   // Encapsulates a single application parameter.
   template<typename charT = char,
            typename traits= std::char_traits<charT> >
   class basic_app_param
   {
      public:         
        static const int NO_ORDER = -1;

      protected:
         using string_t = std::basic_string<charT,traits>; 

         string_t    name_;
         string_t    desc_;
         string_t    alias_;
         int         order_;
         bool        is_required_;
      
      public:

         basic_app_param()
            : name_(),
              desc_(),
              alias_(),
              order_(NO_ORDER),
              is_required_(false)
         {}

         basic_app_param(
            string_t name, 
            string_t desc  = "", 
            string_t alias = "",
            int order      = basic_app_param::NO_ORDER, 
            bool required  = false
            )
            : name_(name),
              desc_(desc),
              alias_(alias),
              order_(order),
              is_required_(required)
         {}

         string_t get_name()        const {return this->name_;}
         string_t get_description() const {return this->description_;}
         string_t get_alias()       const {return this->alias_;}
         int      get_order()       const {return this->order_;}
         bool     get_is_required() const {return this->is_required_;}

         virtual bool parse_value(const string_t & s) = 0;
         virtual bool has_value() const = 0;

         virtual ~basic_app_param()
         {}

   };

   template<typename charT = char,
            typename traits= std::char_traits<charT> >
   class basic_int_param : public basic_app_param<charT,traits>
   {

      private:

         using base_t = basic_app_param<charT,traits>;
         using string_t = std::basic_string<charT,traits>;

         sk3l::core::nullable<int> value_;

      public:
         basic_int_param(
            string_t name, 
            string_t desc  = "", 
            string_t alias = "",
            int order      = base_t::NO_ORDER, 
            bool required  = false
            )
            : base_t(name,desc,alias,order,required)
               
            {}

         bool parse_value(const string_t & t) override 
         {
            int i = 0;
            if (sk3l::core::text::string_util::try_parse(t, i))
            {
               this->value_.set(i);
               return true;
            }
            else
               return false;
         }

         bool has_value() const override
         {
            return this->value_.has_value();
         }

         int get_value() 
         {
            return this->value_.get();
         }

         ~basic_int_param() 
         {}
         
   };

   template<typename charT = char,
            typename traits= std::char_traits<charT> >
   class basic_switch_param : public basic_app_param<charT,traits>
   {

      private:

         using base_t = basic_app_param<charT,traits>;
         using string_t = std::basic_string<charT,traits>;
      
      public:
         basic_switch_param(
            string_t name, 
            string_t desc  = "", 
            string_t alias = "",
            int order      = base_t::NO_ORDER, 
            bool required  = false
            )
            : base_t(name,desc,alias,order,required)
               
            {}

         bool parse_value(const string_t & t) override 
         {
            // no op
            return true;
         }

         bool has_value() const override
         {
            return true; 
         }

         ~basic_switch_param() 
         {}
         
   };


   using switch_app_param = basic_switch_param<char>;
   using wswitch_app_param = basic_switch_param<wchar_t>;

   using int_app_param = basic_int_param<char>;
   using wint_app_param = basic_int_param<wchar_t>;
}
}
#endif // APP_PARAMS_H
