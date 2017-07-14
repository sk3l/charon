#ifndef JSON_DOCUMENT_H
#define JSON_DOCUMENT_H

#include <exception>
#include <fstream>
#include <ostream>
#include <string>
#include <sstream>
#include <typeinfo>

#include "core/text/string_util.h"

#include "json_node.h"
#include "json_object.h"
#include "json_list.h"

namespace sk3l
{
namespace data 
{
namespace xchange 
{

template<typename charT,
         typename traits=std::char_traits<charT>,
         class allocator=std::allocator<json_node<charT,traits> > >
class basic_json_document
{
   private:

      const charT * DECL = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

      using node_t      = xml_node<charT,traits>;
      using node_ptr    = xml_node<charT,traits>*;
      using doc_t       = basic_xml_document<charT,traits,allocator>;
      using element_t   = basic_xml_element_node<charT,traits>;
      using element_it  = typename element_t::iterator;
      using element_ptr = element_t*;
      using element_list= sk3l::data::adt::linked_list<element_t>;
      using elmt_list_it= typename element_list::iterator;
      using string_t    = std::basic_string<charT, traits>;
      using plist_t     = std::vector<string_t>;
      using plist_it    = typename plist_t::iterator;


      string_t  docType_;
      element_t root_;

   public:

      basic_xml_document() 
         : docType_(DECL)
      {}

      explicit basic_xml_document(const element_t & e)
         : docType_(DECL),
           root_(e)
      {}

      explicit basic_xml_document(const string_t & fName)
         : docType_(),
           root_()
      {
         std::ifstream xmlDoc(fName);
         if (!xmlDoc)
         {
            string_t err = "Couldn't open XML document at path '";
            err += fName + "'.";
            throw std::invalid_argument(err);
         }

         std::string line;
         std::stringstream ss;  
         while (xmlDoc.good())
         {
            std::getline(xmlDoc, line);
            ss << line << std::endl; 
         }

         this->deserialize(ss.str());
      }

      element_t & get_root()
      {
         return this->root_;
      }

      element_it find_element(const string_t & s)
      {
        return this->find_element(s, this->begin());
      }

      element_it find_element(const string_t & s, element_it it)
      {
         for (;it != this->end(); ++it)
         {
            //std::cout << "Processing node " << it->get_name() << std::endl;
            if
            (
               typeid(*it) == typeid(element_t) &&
               it->get_name() == s
            )
            {
               break;
            }
         }
 
         return it;
      }

      element_it at(const string_t & path)
      {
         // Split path
         std::vector<string_t> plist = 
            sk3l::core::text::string_util::split(path, '/');

         if (plist.size() < 1)
            return this->end();
         
         element_ptr rptr = &(this->root_);

         plist_it p_cur = plist.begin(),
                  p_end = plist.end();

         if (at_recur(rptr, p_cur, p_end))
         {
            element_it rv(rptr); 
            return rv;
         }
         else
            return this->end();
      }

      string_t serialize() const
      {
         string_t str(basic_xml_document::DECL);
         str += "\n";

         return str + this->root_.to_string(); 
      }  

      void deserialize(const std::string & doc)
      {
         std::basic_stringstream<charT,traits> ss(doc);
         
         std::getline(ss, this->docType_);
         ss >> this->root_;
      }

      element_it begin()   {return this->root_.begin();}
      element_it end()     {return this->root_.end();}

   private:
      
      bool at_recur
      (
         element_ptr & eptr, 
         plist_it & p_cur, 
         plist_it & p_end
      )
      {
         if (eptr == nullptr)
            return false;
         if (p_cur == p_end)
            return false;
         if (eptr->get_name() != *p_cur)
            return false;
         else if (++p_cur == p_end)
            return true;

         auto nptr = eptr->child_nodes_.begin();
         while (nptr != eptr->child_nodes_.end())
         {
            if (nptr->get()->get_type() != node_t::Element)
            {
               ++nptr;
               continue;
            }
            
            eptr = (element_ptr)((nptr++)->get());
            if (at_recur(eptr, p_cur, p_end))
               return true;
         }

         return false;
      } 
};

// Convenience decls
using xml_attribute = basic_xml_attribute_node<char>;
using wxml_attribute = basic_xml_attribute_node<wchar_t> ;

using xml_element = basic_xml_element_node<char>;
using wxml_element = basic_xml_element_node<wchar_t>;

using xml_document =  basic_xml_document<char>;
using wxml_documnent = basic_xml_document<wchar_t>;

template<typename charType,
         typename charTraits=std::char_traits<charType>,
         class alloc=std::allocator<xml_node<charType,charTraits> > >
std::ostream & operator<<(std::ostream & os,
      basic_xml_document<charType,charTraits,alloc> & doc)
{
   for (auto it = doc.get_root().begin(); it != doc.get_root().end(); ++it)
   {
      os << "Node name => " 
         << it->get_name() 
         << ", Node type => " 
         << it->get_type() 
         << std::endl ;
   }    


   return os;
}

}
}
}

#endif // JSON_DOCUMENT_H
