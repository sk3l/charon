#ifndef JSON_NODE_H
#define JSON_NODE_H

#include <string>

namespace sk3l
{
namespace data 
{
namespace xchange 
{

template<typename charT, typename traits=std::char_traits<charT> >
class json_node
{
   public:
      enum Type
      {
         Uknown   = 0,
         Object   = 1,
         String   = 2,
         Number   = 3,
         Array    = 4,
         Boolean  = 5
      };

   using json_node_ptr = json_node*;
   using string_t = std::basic_string<charT,traits>;

   protected:
      json_node::Type type_;

      explicit json_node(json_node::Type t)
        : type_(t)
      {
      }

   public:

      virtual const json_node::Type get_type() const
      {
        return this->type_;
      } 

      virtual string_t get_value() const = 0;

      virtual string_t to_string() const = 0;

      virtual ~json_node()
      {}
};

//using json_node = json_node<char>;
//using json_node_wstr = json_node<wchar_t>;

}

}

}

#endif
