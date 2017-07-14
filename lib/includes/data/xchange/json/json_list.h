#ifndef JSON_LIST_H
#define JSON_LIST_H

#include <memory>
#include <string>
#include <vector>

#include "json_node.h"

namespace sk3l
{
namespace data 
{
namespace xchange 
{

template<typename charT, typename traits=std::char_traits<charT> >
class basic_json_list : public json_node<charT,traits>
{
   public:
      
      using node_t      = json_node<charT,traits>; 
      using base_t      = json_node<charT,traits>;
      using base_ptr_t  = std::shared_ptr<node_t>;
      using string_t    = std::basic_string<charT,traits>;
      using array_t     = std::vector<base_ptr_t>;

      basic_json_list() 
        : base_t(base_t::Type::Array),
          values_()
      {} 

      std::size_t capacity() const  {return this->values_.capacity();}
      std::size_t size() const      {return this->values_.size();}
      void resize(std::size_t s)    {this->value_.resize(s);}
      void clear()                  {this->value_.clear();}

      base_ptr_t operator[](std::size_t idx) {return this->values_[idx];}
      std::uint8_t & at(std::size_t idx)     {return this->values_.at(idx);}
      void push_back(const base_ptr_t & bp)
      {
         this->values_.push_back(bp);
      }
      
      typename array_t::const_iterator begin() const
      {
         return this->values_.begin(); 
      }
      
      typename array_t::iterator begin()
      {
         return this->values_.begin();
      }
      
      typename array_t::const_iterator end() const
      {
          return this->values_.end(); 
      }
      
      typename array_t::iterator end()
      {
          return this->values_.end(); 
      }

      string_t get_value() const override
      {
         return "";
      }

      string_t to_string() const override
      {
         return "";
      }

   private:

      array_t values_;

};

}

}

}

#endif
