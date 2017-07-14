#ifndef JSON_OBJECT_H
#define JSON_OBJECT_H

#include <cctype>
#include <exception>
#include <memory>
#include <string>

#include "json_node.h"
#include "json_list.h"
#include "json_primitive.h"

#include "core/text/string_util.h"
#include "core/io/stream_parser.h"

#include "data/adt/hash_map_o.h"

namespace sk3l
{
namespace data
{
namespace xchange
{

template<typename charT, typename traits=std::char_traits<charT> >
class basic_json_object : public json_node<charT,traits>
{
   public:

      using base_t      = json_node<charT,traits>;
      using base_ptr_t  = std::shared_ptr<base_t>;
      using string_t    = std::basic_string<charT,traits>;
      using str_util_t  = sk3l::core::text::basic_string_util<charT,traits>;
      using parser_t    = sk3l::core::io::stream_parser<charT,traits>;
      using json_obj_t  = basic_json_object<charT,traits>;
      using json_lst_t  = basic_json_list<charT,traits>;
      using json_str_t  = basic_json_string<charT,traits>;
      using json_num_t  = basic_json_number<charT,traits>;
      using json_bool_t = basic_json_bool<charT,traits>;

      basic_json_object()
         : base_t(base_t::Type::Object),
           properties_(1024, sk3l::data::adt::string_hash())
      {}

      ~basic_json_object()
      {}

      string_t get_value() const override
      {
         return "";
      }

      string_t to_string() const override
      {
         sstream_t ss;

         if (this->to_string_recur(*this, ss, (size_t)0))
            return ss.str();

         return "";
      }

      base_ptr_t get_property(const string_t & key)
      {
         return this->properties_[key];
      }

      void add_property(const string_t & key, const base_ptr_t & val)
      {
         return this->properties_.insert(key, val);
      }

      base_ptr_t operator[](const string_t & key)
      {
         return this->properties_[key];
      }

      basic_json_object<charT,traits> * get_object(const string_t & key)
      {
         base_ptr_t val = this->properties_[key];
         if (val == nullptr)
            return nullptr;

         basic_json_object<charT,traits> * obj =
             dynamic_cast<basic_json_object<charT,traits> * >(val.get());

         if (obj == nullptr)
            throw std::invalid_argument("Value with key name '" + key + "'\
 is not a json_object.");

         return obj;
      }

      typedef std::basic_istream<charT,traits> istream_t;
      bool deserialize(istream_t & is)
      {
         this->properties_.clear();
         return this->recur_object(*this, is);
      }

   private:
      sk3l::data::adt::open_hash_map<string_t, base_ptr_t> properties_;

      string_t dequote_string(const string_t & str)
      {
         auto start = str.find("\"");
         if (start == string_t::npos)
            return "";

         auto end = str.rfind("\"");
         if (end == start)
            return "";

         start += 1;
         return str.substr(start, end - start);
      }

      string_t parse_key(istream_t & is)
      {
         parser_t parser(is);
         string_t key;

         parser >> sk3l::core::io::setdelims(":")
                >> sk3l::core::io::noskipws
                >> key;

         if (!is || key.length() < 1)
            return "";

         return this->dequote_string(key);
      }

      bool recur_object
      (
         basic_json_object<charT,traits> & obj,
         istream_t & is
      )
      {
         parser_t parser
         (
            is,
            "",
            false,   // match any, not all
            false,   // discard delims
            true     // preserve whitespace
         );
         parser.discard("{", false);

         if (!is)
            return false;

         while (is)
         {
            string_t key = this->parse_key(is);
            if (key.length() < 1)
               return false;

            string_t val;

            parser.set_delims("{}\"[,");
            parser.set_keep_delims(true);
            parser.skipws();
            val = parser.next_match();

            if (!is)
               return false;

            base_ptr_t jsonNode;
            charT c = parser.get_last_delim();
            //
            // JSON object
            if (c == '{')
            {
               std::unique_ptr<basic_json_object<charT,traits> > childObj
               (
                  new basic_json_object<charT,traits> ()
               );
               this->recur_object(*childObj, is);
               jsonNode.reset(childObj.release());
            }
            //
            // JSON list
            else if (c == '[')
            {
               //parser.discard(1);
               std::unique_ptr<basic_json_list<charT,traits> > childList
               (
                  new basic_json_list<charT,traits> ()
               );

               this->recur_list(*childList, is);
               jsonNode.reset(childList.release());
            }
            //
            // JSON primitive string
            else if (c == '"')
            {
               parser.discard(1);
               parser >> sk3l::core::io::setdelims("\"")
                      >> sk3l::core::io::keepdelims(false)
                      >> val;

               if (!is || val.length() < 1)
                  return false;

               jsonNode.reset(new json_str_t(val));
            }
            //
            // JSON primitive type
            else if (c == ',' || c == '}')
            {
               if (val.length() < 1)
                  return false;

               if (val.compare(0, 4, "true") == 0)
               {
                  jsonNode.reset(new json_bool_t(true));
               }
               else if (val.compare(0, 4, "false") == 0)
               {
                     jsonNode.reset(new json_bool_t(false));
               }
               else if (std::isdigit(val[0]) || (val[0] == '-'))
               {
                  double d;
                  if (str_util_t::template try_parse<double>(val, d))
                     jsonNode.reset(new json_num_t(d));
                  else
                     return false;
               }
               else
               {
                  return false;
               }

            }
            // TO-DO : support null
            else
            {
               // else it's an invalid token
               return false;
            }

            obj.properties_.insert(key, jsonNode);

            parser.set_delims(",}");
            parser.next_match();

            if (!is)
               return false;
            else if (parser.get_last_delim() == '}')
               break;

         }
         return true;
      }

      bool recur_list(basic_json_list<charT,traits> & list, istream_t & is)
      {
         parser_t parser
         (
            is,
            "",
            false,   // match any, not all
            false,   // discard delims
            true     // preserve whitespace
         );
         parser.discard("[", false);

         if (!is)
            return false;

         while (is)
         {
            string_t val;

            parser.set_delims("{\"[],");
            parser.set_keep_delims(true);
            parser.skipws();
            val = parser.next_match();

            if (!is)
               return false;

            base_ptr_t jsonNode;
            charT c = parser.get_last_delim();
            //
            // JSON object
            if (c == '{')
            {
               std::unique_ptr<basic_json_object<charT,traits> > childObj
               (
                  new basic_json_object<charT,traits> ()
               );
               this->recur_object(*childObj, is);
               jsonNode.reset(childObj.release());
            }
            //
            // JSON list
            else if (c == '[')
            {
               //parser.discard(1);
               std::unique_ptr<basic_json_list<charT,traits> > childList
               (
                  new basic_json_list<charT,traits> ()
               );

               this->recur_list(*childList, is);
               jsonNode.reset(childList.release());
            }
            //
            // JSON primitive string
            else if (c == '"')
            {
               parser.discard(1);
               parser >> sk3l::core::io::setdelims("\"")
                      >> sk3l::core::io::keepdelims(false)
                      >> val;

               if (!is || val.length() < 1)
                  return false;

               jsonNode.reset(new json_str_t(val));
            }
            //
            // JSON primitive type
            else if (c == ',' || c == ']')
            {
               if (val.length() < 1)
                  return false;

               if (val.compare(0, 4, "true") == 0)
               {
                  jsonNode.reset(new json_bool_t(true));
               }
               else if (val.compare(0, 4, "false") == 0)
               {
                     jsonNode.reset(new json_bool_t(false));
               }
               else if (std::isdigit(val[0]) || (val[0] == '-'))
               {
                  double d;
                  if (str_util_t::template try_parse<double>(val, d))
                     jsonNode.reset(new json_num_t(d));
                  else
                     return false;
               }
               else
               {
                  return false;
               }

            }
            // TO-DO : support null
            else
            {
               // else it's an invalid token
               return false;
            }

            list.push_back(jsonNode);

            parser.set_delims(",]");
            parser.next_match();

            if (!is)
               return false;
            else if (parser.get_last_delim() == ']')
               break;
            else if (parser.get_last_delim() == ',')
               parser.discard(1);

         }
         return true;
      }

      using sstream_t = std::basic_stringstream<charT,traits>;
      bool to_string_recur
      (
         const json_node<charT,traits> & node,
         sstream_t & ss,
         size_t level,
         bool comma = false
      ) const
      {
         string_t ws(level, ' ');
         level += 3;

         switch (node.get_type())
         {
            case base_t::Type::Object:
            {
               ss << "{" << std::endl;

               const json_obj_t & obj = dynamic_cast<const json_obj_t&>(node);

               size_t s = 0;
               string_t indent = ws + "   ";
               for (auto i : obj.properties_)
               {
                  ss << indent
                     << "\""
                     << i.get_key()
                     << "\""
                     << " : ";
                  this->to_string_recur
                  (
                     *i.get_val(),
                     ss,
                     level,
                     (++s < obj.properties_.size())
                  );
               }
               ss << std::endl << ws << "}";
               if (comma)
               {
                  ss << ",";
                  ss << std::endl;
               }
            }
            break;

            case base_t::Type::Array:
            {
               ss << "[" << std::endl;

               const json_lst_t & obj = dynamic_cast<const json_lst_t&>(node);

               size_t s = 0;
               ss <<  ws + "   ";

               for (const auto i : obj)
               {
                 this->to_string_recur
                  (
                     *i,
                     ss,
                     level,
                     (++s < obj.size())
                  );
               }
               ss << std::endl << ws << " ]";
               if (comma)
               {
                  ss << ",";
                  ss << std::endl;
               }
            }
            break;

            case base_t::Type::String:
               ss << "\""
                  << node.to_string()
                  << "\"";
               if (comma)
                  ss << ", ";
            break;

            case base_t::Type::Number:
            case base_t::Type::Boolean:
               ss << node.to_string();
               if (comma)
                  ss << ", ";
            break;

            default:
               return false;
            break;

         }

         return true;
      }
};

// Convenience decls
using json_object = basic_json_object<char>;
using json_wobject= basic_json_object<wchar_t>;

template<typename charT, typename traits=std::char_traits<charT> >
std::basic_istream<charT,traits> & operator>>
(
 std::basic_istream<charT,traits> & is,
 basic_json_object<charT,traits> & obj
)
{
   obj.deserialize(is);
   return is;
}

}

}

}

#endif
