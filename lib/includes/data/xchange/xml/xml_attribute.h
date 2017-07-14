#ifndef XML_ATTRIBUTE_H
#define XML_ATTRIBUTE_H

#include <string>

#include "xml_node.h"

namespace sk3l
{
namespace data 
{
namespace xchange 
{

template<typename charT, typename traits=std::char_traits<charT> >
class basic_xml_attribute_node : public xml_node<charT,traits>
{
      template<typename T, typename U, typename V>
      friend class basic_xml_tree;

   protected:

      typedef xml_node<charT,traits> base_t;
      typedef std::basic_string<charT,traits> string_t;
      typedef basic_xml_attribute_node<charT,traits> attr_t;

     string_t name_;
     string_t value_;

   public:

      basic_xml_attribute_node(const string_t & name, const string_t & value)
        : base_t(base_t::Type::Attribute),
          name_(name),
          value_(value)
      {}

      basic_xml_attribute_node(const attr_t & attr)
         :  base_t(base_t::Type::Attribute),
            name_(attr.name_),
            value_(attr.value_)
      {} 

      const string_t & get_attribute_name() const
      {
         return this->name_;
      }

      void set_attribute_name(const string_t & n)
      {
         this->name_ = n;
      }

      const string_t & get_attribute_value() const
      {
         return this->value_;
      }

      void set_attribute_value(const string_t & v)
      {
         this->value_ = v;
      }
      
      string_t get_name() const {return this->get_attribute_name();}
      string_t get_value() const {return this->get_attribute_value();}
      
      std::string to_string() const 
      {
         std::string str(" ");
         str += this->name_ + "=";
         str += + "\"" + this->value_ + "\"";
         return str;
      }
};

}

}

}

#endif
