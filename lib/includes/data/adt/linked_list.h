#ifndef LINKED_LIST
#define LINKED_LIST

#include <cstddef>
#include <ostream>
#include <cstddef>

#ifdef __DEBUG__
#include <iostream>
#endif

namespace sk3l
{
namespace data
{
namespace adt
{

template<typename Type>
class linked_list
{
public:
    class linked_list_node
    {
        friend class linked_list;

        using node_type = linked_list_node;
        using value_type= Type;

        private:
            value_type t_;
            node_type * next_;
            node_type * prev_;
            linked_list<Type> * list_;

        public:
            linked_list_node() 
               : t_(),
                 next_(nullptr),
                 prev_(nullptr),
                 list_()
            {}

            linked_list_node(const linked_list_node & rhs)
            : t_(rhs.t_),
              next_(rhs.next_),
              prev_(rhs.prev_),
              list_(rhs.list_)
            {}

            linked_list_node & operator=(const linked_list_node & rhs)
            {
               this->t_ = rhs.t_;
               this->next_ = rhs.next_;
               this->prev_ = rhs.prev_;
               this->list_ = rhs.list_;

               return *this; 
            }

            explicit linked_list_node(const value_type & t)
               : t_(t),
                 next_(nullptr),
                 prev_(nullptr),
                 list_()
            {}

           value_type & get() {return this->t_;}

           void set(const value_type & t) {this->t_ = t;}
#ifdef __DEBUG__
           void print_debug()
           {
              std::cout << "list node prev=" << prev_ << ", next=" << next_
                        << ", list=" << list_ << std::endl;
           }
#endif           
    };

    using node_type = linked_list_node;
    using node_ptr  = linked_list_node*;

    class iterator
    {
      template<class T>
      friend class linked_list;

      using self_type      = iterator;
      using value_type     = Type;
      using reference_type = Type&;
      using pointer_type   = Type*;
      using difference_t   = int;

       public:
      iterator()
         : ptr_(nullptr)
      {}

      iterator(const iterator & rhs)
      : ptr_(rhs.ptr_)
      {
         if 
         (
            this->ptr_  != nullptr &&
            rhs.ptr_    != nullptr
         )
         {
            this->ptr_->list_ = rhs.ptr_->list_;
         }
      }

      explicit iterator(node_ptr p)
         : ptr_(p)
      {
         if 
         (
            this->ptr_  != nullptr &&
            p           != nullptr
         )
         {
            this->ptr_->list_ = p->list_;
         }
      }


      iterator & operator=(const iterator & rhs)
      {
         this->ptr_ = rhs.ptr_;
         if 
         (
            this->ptr_  != nullptr &&
            rhs.ptr_    != nullptr
         )
         {
            this->ptr_->list_ = rhs.ptr_->list_;
         }
 
         return *this;
      };

      iterator & operator++()
      {
         this->ptr_ = this->ptr_->next_;
         return *this;
      } 

      iterator operator++(int ignore)
      {
         self_type copy(*this);
         this->ptr_ = this->ptr_->next_;
         return copy;
      }

      iterator operator--()
      {
         this->ptr_ = this->ptr_->prev_;
         return *this;
      }

      iterator operator--(int ignore)
      {
         self_type copy(*this);
         this->ptr_ = this->ptr_->prev_;
         return copy; 
      }

      reference_type operator*()
      {
        return this->ptr_->t_;
      }

      pointer_type operator->()
      {
         return &(this->ptr_->t_);
      } 

      bool operator==(const iterator & rhs)
      {
         return this->ptr_ == rhs.ptr_;
      }

      bool operator!=(const iterator & rhs)
      {
         return !(*this == rhs);
      }
#ifdef __DEBUG__
      void print_debug()
      {
         ptr_->print_debug();
      }
#endif
       private:

         node_ptr ptr_;  
    };

private:

    using value_type = Type;

    node_ptr head_;
    node_ptr tail_;
    std::size_t size_;

public:
    linked_list() :
        head_(nullptr),
        tail_(nullptr),
        size_(0)
    {}

    linked_list(const linked_list & rhs)
      : head_(nullptr),
        tail_(nullptr),
        size_(rhs.size_)
    {
      if (this->size_ > 0)
      {
         this->head_ = new linked_list_node(*rhs.head_);
         node_ptr prev = this->head_;
         for (auto i = rhs.head_->next_; i != nullptr; i = i->next_)
         {
            prev->next_ = new linked_list_node(*i);
            prev->next_->prev_ = prev;
            prev->next_->list_ = this;
            prev = prev->next_;
         }
         this->tail_ = prev;
      }
    }

    linked_list & operator=(const linked_list & rhs)
    {
      linked_list tmp(rhs);
      std::swap(this->head_, tmp.head_);
      std::swap(this->tail_, tmp.tail_);
      std::swap(this->size_, tmp.size_);
      return *this;      
    }   

    ~linked_list()
    {
      this->clear();
    }

    void push_front(const value_type & t)
    {
        node_ptr newNode = new node_type(t);
        newNode->list_ = this;

        if (this->tail_ == nullptr)
           this->tail_ = newNode;
        
        if (this->head_ != nullptr)
           this->head_->prev_ = newNode;

        newNode->next_ = this->head_;
        this->head_ = newNode;
        ++this->size_;
    }

    void push_back(const value_type & t)
    {
        node_ptr newNode = new node_type(t);
        newNode->list_ = this;

        if (this->head_ == nullptr)
           this->head_ = newNode;
        
        if (this->tail_ != nullptr)
           this->tail_->next_ = newNode;

        newNode->prev_ = this->tail_;
        this->tail_ = newNode;
        ++this->size_;
    }

    void insert(iterator it, const value_type & t)
    {
        if ((it.ptr_ != nullptr) && (it.ptr_->list_ == this))
        {
            node_ptr newNode = new node_type(t);
            newNode->next_ = it.ptr_;
            newNode->prev_ = it.ptr_->prev_;
            if (it.ptr_->prev_)
                 it.ptr_->prev_->next_ = newNode;
            it.ptr_->prev_ = newNode;
            ++this->size_;
        }
    }

    void erase(iterator it)
    {
        if ((it.ptr_ != nullptr) && (it.ptr_->list_ == this))
        {
               if (it.ptr_->prev_ == nullptr)
                    this->head_ = it.ptr_->next_;
                else
                    it.ptr_->prev_->next_ = it.ptr_->next_;

                if (it.ptr_->next_ == nullptr)
                    this->tail_ = it.ptr_->prev_;
                else
                    it.ptr_->next_->prev_ = it.ptr_->prev_;

            delete it.ptr_;
            --this->size_;
        }
    }

    void clear()
    {
       iterator end;
       for (auto it = this->begin(); it != end; --it)
       {
          this->erase(it);
       }
    }

    std::size_t size() const
    {
        return this->size_;
    }

    const iterator begin() const
    {
        return iterator(this->head_);
    }

    iterator begin()
    {
       return iterator(this->head_);
    }

    const iterator end() const
    {
        return iterator();
    }
    
    iterator end()
    {
        return iterator();
    }
};

//template<class T>
//std::ostream& operator<<(const linked_list<T> & ll, std::ostream & o)
//{
//
//}

}
}
}
#endif // LINKED_LIST

