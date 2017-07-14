#ifndef XML_ELEMENT_H
#define XML_ELEMENT_H

#include <memory>
#include <string>
#include <ios>
#include <istream>
#include <limits>
#include <ostream>
#include <sstream>
#include <utility>

#ifndef  NDEBUG 
#include <iostream>
#endif

#include "data/adt/linked_list.h"
#include "data/adt/stack.h"
#include "core/io/stream_parser.h"

#include "xml_attribute.h"
#include "xml_node.h"
#include "xml_text.h"

namespace sk3l
{
namespace data 
{
namespace xchange 
{

template<typename charT, typename traits=std::char_traits<charT> >
class basic_xml_element_node : public xml_node<charT,traits>
{
      template<typename T, typename U, typename V>
      friend class basic_xml_document;

   protected:
      using base_t      = xml_node<charT,traits>;
      using node_t      = xml_node<charT,traits>;
      using element_t   = basic_xml_element_node<charT,traits>;
      using element_ptr = basic_xml_element_node<charT,traits>*;
      using attr_t      = basic_xml_attribute_node<charT,traits>;
      using text_t      = basic_xml_text_node<charT,traits>;
      using string_t    = std::basic_string<charT,traits>;

      using ptr_t       = std::shared_ptr<base_t>;
      using node_list   = sk3l::data::adt::linked_list<ptr_t>; 
      using element_list= sk3l::data::adt::linked_list<element_t>;
      using node_it     = typename sk3l::data::adt::linked_list<ptr_t>::iterator;  
      
      string_t    name_;
      string_t    value_;
      node_list   child_nodes_;

   public:

      basic_xml_element_node()
         :  base_t(base_t::Type::Element),
            name_(""),
            value_("")
      {}      

      explicit basic_xml_element_node(const string_t & name)
        : base_t(base_t::Type::Element),
          name_(name)
      {} 

      explicit basic_xml_element_node(const element_t & e)
        : base_t(base_t::Type::Element),
          value_(e.value_)
      {
         this->name_ = e.name_;
         this->child_nodes_ = e.child_nodes_;
      }

      string_t get_name() const {return this->get_element_name();}
      string_t get_value() const {return this->get_element_value();}
 
      string_t to_string() const 
      {
         std::string str;
         this->to_string_recur(str, 0);
         return str;
      }

      const string_t & get_element_name() const
      {
         return this->name_;
      }

      void set_element_name(const string_t & s)
      {
         this->name_ = s;
      }

      const string_t & get_element_value() const
      {
         return this->value_;
      }

      void set_element_value(const string_t & s)
      {
         this->value_ = s;
      }

      void add_attribute(const attr_t & a)
      {
         this->child_nodes_.push_front(std::shared_ptr<base_t>(new attr_t(a)));
      }

      void add_attribute(const string_t & name, const string_t & val)
      {
         this->add_attribute(attr_t(name, val));
      }

      void add_text(const text_t & txt)
      {
         this->child_nodes_.push_back(std::shared_ptr<base_t>(new text_t(txt)));
      }

      void add_element(const element_t & e)
      {
         this->child_nodes_.push_back(std::shared_ptr<base_t>(new element_t(e)));
      }

      void add_element(const string_t & name)
      {
         this->add_element(element_t(name));
      }

      node_list & get_nodes()
      {
         return this->child_nodes_;
      }

      element_list get_child_elements()
      {
         element_list el;
         
         auto n_beg = this->child_nodes_.begin(),
              n_end = this->child_nodes_.end();

         for(;n_beg != n_end; ++n_beg)
         {
            if (n_beg->get()->get_type() != xml_node<charT>::Element)
               continue;

            auto e_ptr = dynamic_cast<element_ptr>(n_beg->get());
            el.push_back(*e_ptr);
         }

         return el;
      } 
      

      class iterator
      {
         template<typename C, typename T>
         friend class basic_xml_element_node;
         
         using self_type      = iterator;
         using value_type     = base_t;
         using reference_type = base_t&;
         using pointer_type   = base_t*;
         using difference_t   = int;

         struct iter_state
         {
            element_ptr host_;
            node_it     next_;

            iter_state()
               : host_(nullptr),
                 next_(nullptr)
            {}

            iter_state(const iter_state & rhs)
               : host_(rhs.host_),
                 next_(rhs.next_)
            {}

            explicit iter_state(element_ptr e)
               : host_(e)
            {
               if (e != nullptr)
                  this->next_ = e->child_nodes_.begin();   
            }
         };
         
         using iter_stack_t = sk3l::data::adt::stack<iter_state>;

         // iterator member vars
         pointer_type   node_ptr_;
         node_it        node_it_;
         iter_state     curr_;
         iter_stack_t   trail_; 

         public:
         explicit iterator(element_ptr e)
          : node_ptr_(e),
            node_it_(nullptr),
            curr_(e),
            trail_()
         {}

         iterator(const iterator & rhs)
         :  node_ptr_(rhs.node_ptr_),
            node_it_(rhs.node_it_),
            curr_(rhs.curr_),
            trail_(rhs.trail_)
         {}

         iterator & operator=(const iterator & rhs)
         {
            this->node_ptr_   = rhs.node_ptr_;
            this->node_it_    = rhs.node_it_;
            this->curr_       = rhs.curr_;
            this->trail_      = rhs.trail_;
            return *this;
         };

         iterator operator++()
         {
            return this->advance();
         } 

         iterator operator++(int ignore)
         {
            self_type copy(*this);
            this->advance();
            return copy; 
         }

         iterator operator--()
         {
            return *this;
         }

         iterator operator--(int ignore)
         {
            self_type copy(*this);
            return copy; 
         }

         reference_type operator*()
         {
            return *(this->node_ptr_);
         }
   
         pointer_type operator->()
         {
            return this->node_ptr_;
         } 

         bool operator==(const iterator & rhs)
         {
            return this->node_ptr_ == rhs.node_ptr_;
         }

         bool operator!=(const iterator & rhs)
         {
            return !(*this == rhs);
         }

         iterator advance()
         {
            if (this->node_ptr_->get_type() == base_t::Type::Element)
            {
               element_ptr e = dynamic_cast<element_ptr>(this->node_ptr_);

               if (e->child_nodes_.size() > 0)
               {
                  this->node_it_  = e->child_nodes_.begin();
                  this->node_ptr_ = e->child_nodes_.begin()->get();

                  if (this->node_ptr_->get_type() == base_t::Type::Element)
                  {
                     trail_.push(curr_);
                     e = dynamic_cast<element_ptr>(this->node_ptr_);
                     this->curr_ = iter_state(e); 
                  }
                  else
                  {
                     ++this->curr_.next_;
                  }
              
                  return *this;
               }
               
            }
            
            if (this->curr_.next_ != this->curr_.host_->child_nodes_.end())
            {
              this->node_it_ = this->curr_.next_;
              this->node_ptr_ = this->curr_.next_->get();

              ++this->curr_.next_;
               return *this;
            }
          
            this->node_ptr_ = nullptr; 
            while (this->trail_.size() > 0)
            {
               this->curr_ = this->trail_.pop();
               if (this->curr_.next_ != this->curr_.host_->child_nodes_.end())
               {
                  this->curr_.next_++;
                  this->node_it_  = this->curr_.next_;
                  this->node_ptr_ = this->curr_.next_->get();
                  return *this;
               }
            }
            
            return *this;
         } 

    };

   using element_it  = typename basic_xml_element_node<charT,traits>::iterator;
   
   bool erase(element_it & e)
   {
      element_ptr ep = e.curr_.host_;
      node_it i      = e.node_it_;

      if (ep == e.node_ptr_)
      {
         while (e.trail_.size() > 0)
         {
            e.curr_ = e.trail_.pop();
            ep = dynamic_cast<element_ptr>(e.curr_.next_->get());
            if (ep == e.node_ptr_)
               ep = e.curr_.host_;
         }
      }
      
      ep->child_nodes_.erase(i);
      return true;
   }
   
   iterator begin() {return iterator(this);}
   iterator end() {return iterator(nullptr);}

   typedef std::basic_istream<charT,traits> istream_t;
   bool deserialize(istream_t & is)
   {
      this->name_.clear();
      this->value_.clear();
      this->child_nodes_.clear();

      string_t nxt;
      sk3l::core::io::stream_parser<charT,traits> parser(is);
      parser.discard("<");
      parser >> sk3l::core::io::setdelims(">") >> sk3l::core::io::noskipws >> nxt;

      if (!is || nxt.size() < 4)
         return false;

      auto it = nxt.begin();
      if (*it++ != '<')
         // Malformed element (first non-ws must be '<')
         return false;

      if (*it == '/')
         // Hit end tag from parent element.
         return false;

      if (*it == '!' && *(it+1) == '-')
         // Hit a comment
         return false;

      if (*it == '?')
      {
         // Skip XML doc decl, if present
         parser.discard("<"); 
         parser >> sk3l::core::io::setdelims(">") >>  nxt;
         it = nxt.begin() + 1;
      }

      // Extract element name
      while (is && it != nxt.end()) 
      {
         if (*it == '/')
            return true;
         else if (*it == '"' || *it == '\'')
         {
            ++it;
            continue;
         }
          else if (std::isspace(*it))
            break;

         this->name_.push_back(*it);
         ++it;
      }

      // Extract attributes, if present
      while (is && it != nxt.end())
      {
         if (*it == '/')
            return true;

         if (std::isspace(*it))
         {
            ++it;
            continue;
         }

         attr_t a("","");

         // Extract attr name
         auto nend = std::find(it, nxt.end(), '=');

         if (nend == nxt.end())
            return false;       
         std::string name(it, nend);
         it = nend+1;
         if (*it != '"' && *it != '\'')
            return false;
      
         // Extract attr value
         auto vend = 
            std::find_if
            (
               ++it, 
               nxt.end(), 
               [](charT c) 
               {
                  return (c == '"' || c == '\'');
               }
            );
         std::string val(it, vend);

         this->child_nodes_.push_front(ptr_t(new attr_t(name, val)));
         it = vend+1;            
      }

      while (is)
      {
         // Parse any text
         parser   >> sk3l::core::io::keepdelims(true) 
                  >> sk3l::core::io::skipws
                  >> sk3l::core::io::setdelims("<") 
                  >> nxt;
         if (nxt.size() > 0)
         {
            this->set_element_value(nxt);
         }

         // Parse any comment 
         string_t comment = parser.peek(4);
         if (comment == "<!--")
         {
            parser.discard(4);

            parser   >> sk3l::core::io::setdelims("-->")
                     >> sk3l::core::io::keepdelims(false) 
                     >> sk3l::core::io::skipws
                     >> sk3l::core::io::matchall(true) 
                     >> nxt;

            this->child_nodes_.push_back(ptr_t(new text_t(nxt)));
         }  

         // Parse child element
         std::unique_ptr<element_t> e(new element_t());
         if (!e->deserialize(is))
            break;
     
         this->child_nodes_.push_back(ptr_t(e.release()));
      }
     
      return true;
   }


   ~basic_xml_element_node()
   {}

   private:

      void to_string_recur(std::string & s, size_t level) const 
      {
         bool closed = false;
         std::string ws(level, ' ');

         s += ws + std::string("<") + this->get_name();

         for 
         (
            auto i = this->child_nodes_.begin(); 
            i != this->child_nodes_.end(); 
            ++i
         )
         {
            switch (i->get()->get_type())
            {
               default:
               case node_t::Attribute:
                  s += i->get()->to_string();
               break;
               
               case node_t::Text:
                  if (!closed)
                  {
                     s += string_t(">") + "\n";
                     closed = true;
                  }
                  s += ws + i->get()->to_string() +  ws + "\n";
               break;

               case node_t::Element:
                  if (!closed)
                  {
                     s += ">\n";
                     closed = true;
                  }
                  element_ptr e = dynamic_cast<element_ptr>(i->get());
                 
                  e->to_string_recur(s, level+3);
               break;
            }
         }

         if (!closed && this->get_value().empty())
         {
            s += "/>\n";
            closed = true;
         }
         else 
         {
            if (!this->get_value().empty())
            {
               //s += ">\n";
               s += ">";
               s += /*ws +*/ this->get_value();// + "\n"; 
               s += std::string("</") + this->get_name() + std::string(">\n");
            }
            else
               s += ws + std::string("</") + this->get_name() + std::string(">\n");
         }
      }

#ifndef NDEBUG      
      public:
      void print_debug()
      {
         std::cout << "Element " << get_name() << ": " << child_nodes_.size()
                   << "nodes" << std::endl;

         for (auto i = child_nodes_.begin(); i != child_nodes_.end();  ++i)
         {
            i.print_debug();
            if (i->get()->get_type() == base_t::Type::Element)
            {
               element_ptr e = dynamic_cast<element_ptr>(i->get());
               e->print_debug();
            }
            else
            {
               std::cout << "## XML node name: " << i->get()->get_name() << " : type "
                         << i->get()->get_type() << std::endl;
            }
         }
      }
#endif      
};

template<typename charT, typename traits=std::char_traits<charT> >
std::basic_istream<charT,traits> & operator>>
(
 std::basic_istream<charT,traits> & is,
 basic_xml_element_node<charT,traits> & element
)
{
   element.deserialize(is);
   return is;
}

template<typename charT, typename traits=std::char_traits<charT> >
std::basic_ostream<charT,traits> & operator<<
(
 std::basic_ostream<charT,traits> & os,
 const basic_xml_element_node<charT,traits> & element
)
{
   return os << element.to_string();
}

}

}

}

#endif
