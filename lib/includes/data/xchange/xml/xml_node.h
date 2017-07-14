#ifndef XML_NODE_H
#define XML_NODE_H

#include <string>

namespace sk3l
{
namespace data 
{
namespace xchange 
{

template<typename charT, typename traits=std::char_traits<charT> >
class xml_node
{
   public:

      enum Type
      {
         Uknown      = 0,
         Element     = 1,
         Attribute   = 2,
         Text        = 3,
         Comment     = 4
      };

   typedef xml_node* xml_node_ptr;
   typedef std::basic_string<charT,traits> string_t;

   protected:
      xml_node::Type type_;

      explicit xml_node(xml_node::Type t)
        : type_(t)
      {
      }

   public:

      virtual const xml_node::Type get_type() const
      {
        return this->type_;
      } 

      virtual string_t get_name() const = 0;
      virtual string_t get_value() const = 0;

      virtual string_t to_string() const = 0;

      virtual ~xml_node()
      {}
};

}

}

}

#endif
