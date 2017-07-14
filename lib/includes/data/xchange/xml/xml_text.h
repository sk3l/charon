#ifndef XML_TEXT_H
#define XML_TEXT_H

#include <string>

#include "xml_node.h"

namespace sk3l
{
namespace data 
{
namespace xchange 
{

template<typename charT, typename traits=std::char_traits<charT> >
class basic_xml_text_node : public xml_node<charT,traits>
{
      template<typename T, typename U, typename V>
      friend class basic_xml_document;

   protected:
      typedef xml_node<charT,traits> base_t;
      typedef std::basic_string<charT,traits> string_t;
      typedef basic_xml_text_node<charT,traits> text_t;

      string_t value_;

   public:

      explicit basic_xml_text_node(const string_t & s)
         :  base_t(base_t::Type::Text),
            value_(s)
      {}

      const string_t & get_text() const
      {
         return this->value_;
      }

      void set_text(const string_t & s)
      {
         this->value_ = s;
      }

      string_t get_name() const {return string_t();}
      string_t get_value() const {return this->get_text();}

      string_t to_string() const 
      {
         std::string str("<!--");
         str += this->value_;
         str += "-->";  
         return str; 
      }
};

using xml_text = basic_xml_text_node<char>;
using wxml_text= basic_xml_text_node<wchar_t>;

}

}

}

#endif
