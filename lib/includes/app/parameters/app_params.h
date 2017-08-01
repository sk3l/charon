#ifndef APP_PARAMS_H
#define APP_PARAMS_H

#include <string>

#include "core/nullable.h"
#include "core/text/string_util.h"

namespace sk3l {
namespace app {

   enum param_type
   {
      NONE = -1,
      POSITIONAL,
      OPTIONAL
   };

   // Encapsulates a single application parameter.
   template<typename charT = char,
            typename traits= std::char_traits<charT> >
   class basic_app_param
   {
      public:

      protected:
         using string_t = std::basic_string<charT,traits>;

         string_t    name_;
         string_t    desc_;
         string_t    value_;

      public:

         basic_app_param
         (
            const string_t & name,
            const string_t & desc,
            const string_t & val
         )
           : name_(name),
             desc_(desc),
             value_(val)
         {}

         basic_app_param(const basic_app_param & rhs)
            : name_(rhs.name_),
              desc_(rhs.desc_),
              value_(rhs.value_)
         {}

         basic_app_param & operator=(const basic_app_param & rhs)
         {
            if (this != &rhs)
            {
               this->name_ = rhs.name_;
               this->desc_ = rhs.desc_;
               this->value_ = rhs.value_;
            }

            return *this;
         }

         string_t get_name()  const {return this->name_;}
         string_t get_desc()  const {return this->desc_;}
         string_t get_value() const {return this->value_;}

         void     set_value(const string_t & s) {this->value_ = s;}

         virtual string_t   get_usage_str()  const = 0;
         virtual param_type get_param_type() const = 0;

         virtual ~basic_app_param()
         {
         }

   };

   template<typename charT = char,
            typename traits= std::char_traits<charT> >
   class basic_optional_param : public basic_app_param<charT,traits>
   {
      private:

         using string_t = std::basic_string<charT,traits>;
         using base_t   = basic_app_param<charT,traits>;

         string_t    long_option_;
         string_t    short_option_;
         bool        is_switch_;    // Is it boolean on/off with no value?

      public:
         basic_optional_param(
            const string_t & name,
            const string_t & desc,
            const string_t & lopt,
            const string_t & sopt,
            bool is_switch = false
         )
         :
           base_t(name, desc, ""),
           long_option_(lopt),
           short_option_(sopt),
           is_switch_(is_switch)
         {}

         basic_optional_param(const basic_optional_param & rhs)
         :
            base_t(rhs.name_, rhs.desc_, rhs.value_),
            long_option_(rhs.long_option_),
            short_option_(rhs.short_option_),
            is_switch_(rhs.is_switch_)
         {
         }

         basic_optional_param & operator=(const basic_optional_param & rhs)
         {
            if (this != &rhs)
            {
               this->name_          = rhs.name_;
               this->desc_          = rhs.desc_;
               this->long_option_   = rhs.long_option_;
               this->short_option_  = rhs.short_option_;
               this->is_switch_     = rhs.is_switch_;
               this->value_         = rhs.value_;
            }

            return *this;
         }

         string_t    get_long_option()    const {return this->long_option_;}
         string_t    get_short_option()   const {return this->short_option_;}
         bool        get_is_switch()      const {return this->is_switch_;}

         string_t get_usage_str() const override
         {
            return "[--" + this->name_ + "]";
         }

         param_type get_param_type() const override
         {
            return param_type::OPTIONAL;
         }

         ~basic_optional_param()
         {}

   };

   template<typename charT = char,
            typename traits= std::char_traits<charT> >
   class basic_positional_param : public basic_app_param<charT,traits>
   {
      private:

         using string_t = std::basic_string<charT,traits>;
         using base_t   = basic_app_param<charT,traits>;

         int position_;

      public:

         basic_positional_param()
         {}

         basic_positional_param
         (
            const string_t & name,
            const string_t & desc,
            int pos
         )
            : base_t(name, desc, ""),
              position_(pos)
         {}

         basic_positional_param(const basic_positional_param & rhs)
            : base_t(rhs.name_, rhs.desc_, rhs.value_),
              position_(rhs.position_)
         {
            this->position_      = rhs.position_;
         }

         basic_positional_param & operator=(const basic_positional_param rhs)
         {
            if (this != &rhs)
            {
               this->name_          = rhs.name_;
               this->desc_          = rhs.desc_;
               this->position_      = rhs.position_;
               this->value_         = rhs.value_;
            }

            return *this;
         }

         void set_position(int p)  {this->position_ = p;}
         int  get_position() const {return this->position_;}

         string_t get_usage_str() const override
         {
            return "<" + this->name_ + ">";
         }

         param_type get_param_type() const override
         {
            return param_type::POSITIONAL;
         }

         ~basic_positional_param()
         {}

   };

   using app_param = basic_app_param<char>;
   using wapp_param= basic_app_param<wchar_t>;

   using optional_param = basic_optional_param<char>;
   using woptional_param= basic_optional_param<wchar_t>;

   using positional_param = basic_positional_param<char>;
   using wpositional_param= basic_positional_param<wchar_t>;
}
}
#endif // APP_PARAMS_H
