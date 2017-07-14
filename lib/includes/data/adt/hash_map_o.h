#ifndef HASHLINK_H
#define HASHLINK_H

#include <cstddef>
#include <exception>
#include <functional>
#include <utility>

namespace sk3l
{
namespace data
{
namespace adt
{

template<typename Key, typename Type>
class open_hash_map
{
   public:
       using key_t = Key;
       using value_t = Type;

   private:

       static constexpr double LOAD_FACTOR = 0.8;
       static const size_t DEFAULT_CAPACITY = 1024;


       class open_hash_map_node
       {
          friend class open_hash_map;

          private:
             using hcn_t = open_hash_map_node;

             key_t    key_;
             value_t  val_;
             hcn_t  * nxt_;

          public:
             open_hash_map_node()
                :
                key_(), val_(), nxt_(nullptr)
             {}

             open_hash_map_node(key_t k, const value_t & v)
                :
                key_(k), val_(v), nxt_(nullptr)
             {}

             open_hash_map_node(const open_hash_map_node & rhs)
             : key_(rhs.key_),
               val_(rhs.val_),
               nxt_(nullptr)
             {}

             open_hash_map_node & operator=(const open_hash_map_node & rhs)
             {
                if (this != &rhs)
                {
                   key_ = rhs.key_;
                   val_ = rhs.val_;
                   nxt_ = nullptr;
                }

                return *this;
             }

             ~open_hash_map_node()
             {}

             key_t      get_key() const {return this->key_;}
             const value_t &  get_val() const {return this->val_;}
             value_t &  get_val() {return this->val_;}

      };

      using node_t      = open_hash_map_node;
      using node_ptr_t  = open_hash_map_node*;
      using hash_fn_t   = std::function<int(key_t)>;

      std::size_t capacity_;
      std::size_t size_;
      node_t ** slots_;
      hash_fn_t hash_fn_;

      void grow()
      {
         size_t oldCap     = this->capacity_;
         node_t ** oldSlots= this->slots_;

         try
         {
            this->capacity_ *= 2;
            node_ptr_t * newSlots = new node_ptr_t[this->capacity_]();

            for (size_t i = 0; i < oldCap; ++i)
            {
               node_ptr_t np = oldSlots[i];
               while (np != nullptr)
               {
                  this->insert_node(np, newSlots);
                  np = np->nxt_;
               }
            }

            this->delete_nodes();
            delete [] this->slots_;
            this->slots_ = newSlots;
         }
         catch (const std::exception & err)
         {
            // Rollback internal state
            this->capacity_      = oldCap;
            this->slots_         = oldSlots;
            throw;
         }

      }

      class default_hash
      {
         int operator()(key_t t)
         {
            return static_cast<int>(t);
         }
      };

      bool insert_node(node_ptr_t newnode, node_ptr_t * & slots)
      {
         int idx = this->hash_fn_(newnode->key_) % this->capacity_;
         node_ptr_t np = slots[idx];

         if (np == nullptr)
         {
            slots[idx] = newnode;
         }
         else
         {
            do
            {
               if (np->key_ == newnode->key_)
                  return false;
               np = np->nxt_;
            }
            while (np != nullptr);

            np = slots[idx];
            slots[idx] = newnode;
            slots[idx]->nxt_ = np;
         }

         ++this->size_;
         return true;
      }

      void delete_nodes()
      {
        for (size_t i = 0; i < this->capacity_; ++i)
        {
           if (this->slots_[i] != nullptr)
           {
              node_ptr_t np = this->slots_[i];

              while (np != nullptr)
              {
                  node_ptr_t tmp = np->nxt_;
                  delete np;
                  np = tmp;
              }
              this->slots_[i] = nullptr;
           }
        }
      }

      void copy_nodes(const open_hash_map & rhs)
      {
         for (size_t i = 0; i < this->capacity_ && i < rhs.capacity_; ++i)
         {
            if (rhs.slots_[i] != nullptr)
            {
               node_ptr_t src = rhs.slots_[i];
               node_ptr_t cpy = new node_t(*src);
               node_ptr_t nxt = cpy;

               while (src->nxt_ != nullptr)
               {
                  nxt->nxt_ = new node_t(*src->nxt_);
                  nxt = nxt->nxt_;
                  src = src->nxt_;
               }

               this->slots_[i] = cpy;
            }
            else
            {
               this->slots_[i] = nullptr;
            }
         }
      }

   public:

      open_hash_map(size_t c = DEFAULT_CAPACITY, hash_fn_t defHash = default_hash())
        : capacity_(c),
          size_(0),
          slots_(new node_ptr_t[c]()),
          hash_fn_(defHash)
      {}

      open_hash_map(const open_hash_map & rhs)
        : capacity_(rhs.capacity_),
          size_(rhs.size_),
          slots_(new node_ptr_t[capacity_]()),
          hash_fn_(rhs.hash_fn_)
      {
         this->copy_nodes(rhs);
      }

      open_hash_map & operator=(const open_hash_map & rhs)
      {
         if (this != &rhs)
         {
            this->delete_nodes();
            delete [] slots_;

            this->capacity_ = rhs.capacity_;
            this->size_     = rhs.size_;
            this->slots_    = new node_ptr_t[capacity_]();
            this->hash_fn_  = rhs.hash_fn_;

            this->copy_nodes(rhs);
         }
         return *this;
      }

      ~open_hash_map()
      {
         this->delete_nodes();
         delete [] slots_;
      }

      void clear()
      {
         this->delete_nodes();
         this->size_ = 0;
      }

      bool insert(key_t k, const value_t & v)
      {
         node_ptr_t newnode = new node_t(k, v);
         return this->insert_node(newnode, this->slots_);
      }

      bool exists(key_t k)
      {
         int idx = this->hash_fn_(k) % this->capacity_;
         node_ptr_t np = this->slots_[idx];

         if (np == nullptr)
         {
            return false;
         }

         do
         {
            if (np->key_ == k)
            {
               return true;
            }
            np = np->nxt_;
         }
         while (np != nullptr);

         return false;
      }

      class iterator
      {
         friend class open_hash_map;

         using self_type   = iterator;
         using value_type  = node_t;
         using ref_type    = node_t&;
         using pointer_type= node_t*;
         using differenc_t = int;

         public:

         iterator()
            : it_slots_(nullptr), ptr_(nullptr), slot_idx_(-1), slot_cnt_(0)
         {}

         iterator(node_ptr_t p)
            : it_slots_(nullptr), ptr_(p), slot_idx_(-1), slot_cnt_(0)
         {}

         iterator(const iterator & rhs)
         :
            it_slots_(rhs.it_slots_),
            ptr_(rhs.ptr_),
            slot_idx_(rhs.slot_idx_),
            slot_cnt_(rhs.slot_cnt_)
         {}

         iterator operator=(const iterator & rhs)
         {
            this->it_slots_   = rhs.it_slots_;
            this->ptr_        = rhs.ptr_;
            this->slot_idx_   = rhs.slot_idx_;
            this->slot_cnt_   = rhs.slot_cnt_;
            return *this;
         };

         iterator operator++()
         {
            this->ptr_ = this->next();

            return *this;
         }

         iterator operator++(int ignore)
         {
            iterator tmp(*this);
            this->ptr = this->next();
            return tmp;
         }

         ref_type operator*()
         {
            return *this->ptr_;
         }

         pointer_type operator->()
         {
            return this->ptr_;
         }

         bool operator==(const iterator & rhs)
         {
            return this->ptr_ == rhs.ptr_;
         }

         bool operator!=(const iterator & rhs)
         {
            return !(*this == rhs);
         }

         private:

         node_ptr_t next()
         {
	         node_ptr_t nxt = nullptr;

	         if (this->ptr_->nxt_ != nullptr)
	         {
               nxt = this->ptr_->nxt_;
            }
            else
            {
               while (++this->slot_idx_ < this->slot_cnt_)
               {
                  nxt = this->it_slots_[this->slot_idx_];
                  if (nxt != nullptr)
                     break;
               }
            }

            return nxt;
         }

         node_ptr_t *   it_slots_;
         node_ptr_t     ptr_;
         int            slot_idx_;
         int            slot_cnt_;
    };

    iterator find(key_t k) const
    {
      int idx = this->hash_fn_(k) % this->capacity_;
      iterator it(this->slots_[idx]);
      it.it_slots_ = this->slots_;
      it.slot_cnt_ = this->capacity_;
      it.slot_idx_ = idx;

      if (it != nullptr)
      {
         do
         {
            if (it.ptr_->key_ == k)
               break;
            it.ptr_ = it.ptr_->nxt_;
         }
         while (it != nullptr);
      }

      return it;
    }

    value_t & operator[](const key_t & key)
    {
      return this->find(key)->get_val();
    }

   iterator begin()
   {
      iterator it(nullptr);
      it.it_slots_   = this->slots_;
      it.slot_idx_   = 0;
      it.slot_cnt_   = this->capacity_;

      for (; it.slot_idx_ < it.slot_cnt_; ++it.slot_idx_)
      {
         it.ptr_ = this->slots_[it.slot_idx_];
         if (it.ptr_ != nullptr)
            break;
      }

      return it;
   }

   const iterator begin() const
   {
      iterator it(nullptr);
      it.it_slots_   = this->slots_;
      it.slot_idx_   = 0;
      it.slot_cnt_   = this->capacity_;

      for (; it.slot_idx_ < it.slot_cnt_; ++it.slot_idx_)
      {
         it.ptr_ = this->slots_[it.slot_idx_];
         if (it.ptr_ != nullptr)
            break;
      }

      return it;
   }

   iterator end()
   {
      iterator it(nullptr);
      it.it_slots_   = this->slots_;
      it.slot_idx_= it.slot_cnt_ = this->capacity_;
      return it;
   }

   const iterator end() const
   {
      iterator it(nullptr);
      it.it_slots_   = this->slots_;
      it.slot_idx_= it.slot_cnt_ = this->capacity_;
      return it;
   }

   const size_t size() const
   {
      return this->size_;
   }

   const size_t capacity() const
   {
      return this->capacity_;
   }
};

//template<typename charT=char, typename traits=std::char_traits<charT> >
struct string_hash
{
   int operator()(std::string s) const
   {
      size_t hsh = reinterpret_cast<size_t>(s.c_str());

      for (size_t i = 0; i < s.length(); ++i)
      {
         int c = s[i];
         hsh += (hsh ^ c);
      }

      return hsh;
   }
};


}
}
}

#endif
