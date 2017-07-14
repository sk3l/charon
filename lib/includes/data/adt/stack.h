#ifndef STACK_H
#define STACK_H

#include <cstddef>
#include <exception> 
#include <utility>

namespace sk3l
{
namespace data
{
namespace adt
{

template<typename Type>
class stack
{

private:

   template<typename T>
   class stack_entry
   {
      friend class stack;

      private:
         typedef T stack_type;

         T value_;
         stack_entry * next_;

      public:
         stack_entry()
            : value_(),
              next_(nullptr)
         {}

         stack_entry(const stack_type & st, stack_entry * next = nullptr)
           : value_(st),
             next_(next)
         {}

         stack_entry(const stack_entry & se)
         : value_(se.value_),
           next_(nullptr) // don't want shallow copy of next ptr
         {}

         stack_entry & operator=(const stack_entry & rhs)
         {
            this->value_ = rhs.value_;
            this->next_ = nullptr;
            return *this;
         }

        stack_entry(stack_entry && rhs)
        : value_(std::move(rhs.value_)),
          next_(rhs.next) 
        {
           rhs.next_ = nullptr;
        }

   };

   typedef stack_entry<Type> entry;

   entry * top_;
   std::size_t size_;

public:

   stack()
      : top_(nullptr), size_(0)
   {}

   stack(const stack & rhs)
   : top_(nullptr),
     size_(rhs.size_)
   {
      if (this->size_ > 0)
      {
         this->top_ = new entry(*rhs.top_);
         entry * prev = this->top_;
         for (entry * e = rhs.top_->next_; e != nullptr; e = e->next_)
         {
            entry * curr = new entry(*e);
            prev->next_ = curr;
            prev = curr; 
         }
      }  
   }

   stack & operator=(const stack & rhs)
   {
      stack tmp(rhs);
      std::swap(this->size_, tmp.size_);
      std::swap(this->top_, tmp.top_);
      return *this;      
   }

   ~stack()
   {
      while (this->top_ != nullptr)
      {
         entry * tmp = this->top_;
         this->top_ = this->top_->next_;
         delete tmp;
      }
   }

   Type pop() 
   {
      if (this->size_ < 1)
         throw std::out_of_range("Attempt made to pop() empty stack.");

      entry * e = this->top_;
      this->top_ = this->top_->next_;
      --this->size_;
      
      Type t = std::move(e->value_);
      delete e;

      return t;
   };

   void push(const Type & t)
   {
      entry * e = new entry(t, this->top_);
      this->top_ = e;
      ++this->size_;
   }

   std::size_t size() const
   {
      return this->size_;
   }

};

}
}
}
#endif // STACK_H

