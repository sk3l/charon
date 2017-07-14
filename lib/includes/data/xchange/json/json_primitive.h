#ifndef JSON_PRIMITIVE_H
#define JSON_PRIMITIVE_H

#include <string>
#include <sstream>

#include "json_node.h"

namespace sk3l
{
namespace data 
{
namespace xchange 
{

// JSON string
template<class charT=char, class traits=std::char_traits<charT> >
class basic_json_string : public json_node<charT,traits>
{
   using string_t = std::basic_string<charT,traits>;
   using base_t   = json_node<charT,traits>;
      
   private:
      string_t value_;

   public:
      explicit basic_json_string(const string_t & v) :
         base_t(base_t::Type::String),
         value_(v)
      {}

      basic_json_string & operator=(const string_t & rhs)
      {
         this->value_ = rhs;
         return *this; 
      }

      string_t get_value() const override 
      {
         return this->value_;
      }
 
      string_t to_string() const override 
      {
        return this->value_;
      }
};

// JSON numeric
template<class charT=char, class traits=std::char_traits<charT> >
class basic_json_number : public json_node<charT,traits>
{
   using string_t = std::basic_string<charT,traits>;
   using base_t   = json_node<charT,traits>;
      
   private:
      double value_;

   public:
      explicit basic_json_number(double v) :
         base_t(base_t::Type::Number),
         value_(v)
      {}

      basic_json_number & operator=(double rhs)
      {
         this->value_ = rhs;
         return *this; 
      }

      string_t get_value() const override 
      {
         return this->to_string();
      } 
 
      string_t to_string() const override 
      {
         std::stringstream ss;
         ss << this->value_;
         return ss.str();
      }

};

// JSON boolean 
template<class charT=char, class traits=std::char_traits<charT> >
class basic_json_bool : public json_node<charT,traits>
{
   using string_t = std::basic_string<charT,traits>;
   using base_t   = json_node<charT,traits>;
      
   private:
      bool value_;

   public:
      explicit basic_json_bool(bool v) :
         base_t(base_t::Type::Boolean),
         value_(v)
      {}

      basic_json_bool & operator=(bool rhs)
      {
         this->value_ = rhs;
         return *this; 
      }

      string_t get_value() const override 
      {
         return this->to_string();
      } 
 
      string_t to_string() const override 
      {
         std::stringstream ss;
         ss << this->value_;
         return ss.str();
      }

};

// Convenience decls
using json_string = basic_json_string<char>;
using json_wstring = basic_json_string<wchar_t>;

using json_number = basic_json_number<char>;
using json_wnumber= basic_json_number<wchar_t>;

using json_bool = basic_json_bool<char>;
using json_wbool= basic_json_bool<wchar_t>;
}

}

}
#endif
