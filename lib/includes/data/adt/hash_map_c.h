#ifndef HASHMAP_H
#define HASHMAP_H

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
class hash_map
{
   public:
   typedef Type value_type;
   typedef Key key_type;
 
private:

   const int CAPACITY_LIST [26] = 
   {
      53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317,
      196613, 393241, 786433, 1572869, 3145739, 6291469, 12582917, 25165843,
      50331653, 100663319, 201326611, 402653189, 805306457, 1610612741 
   };

   const double LOAD_FACTOR = 0.8;

   class hash_map_entry
   {
      friend class hash_map;

      private:
        key_type key_;
        value_type value_;

        hash_map_entry * nxt_;

      public:
         hash_map_entry()
          : nxt_(nullptr)
         {}

         hash_map_entry(const key_type & k, const value_type & v)
            : key_(k),
              value_(v),
              nxt_(nullptr)
         {}
   };

   struct default_hash
   {
      int operator()(Key k) const
      {
         return static_cast<int>(k);
      }
   };

   struct aux_hash
   {
      int operator()(int i, int j) const
      {
         return (1 + (i % (j - 1)));  
      }
   };


   typedef hash_map_entry entry;
   typedef hash_map_entry* entry_ptr;
   typedef std::function<int(Key)> hash_fn;
  
   entry_ptr* slots_; 
   entry_ptr free_list_;

   std::size_t capacity_;
   std::size_t capacity_idx_;
   std::size_t size_;

   hash_fn hash_;

   void grow()
   {
      size_t oldCap = this->capacity_;
      size_t oldIdx = this->capacity_idx_;
      entry_ptr * oldSlots = this->slots_;

      try
      {
         this->capacity_idx_ += 1;
         this->capacity_ = CAPACITY_LIST[this->capacity_idx_];
   
         entry_ptr * newSlots = new entry_ptr[this->capacity_];
         for (size_t i = 0; i < oldCap; ++i)
         {
            newSlots[i] = this->slots_[i];
         }
   
         for (size_t i = oldCap; i < this->capacity_; ++i)
         {
            entry_ptr entry = new hash_map_entry();
            entry->nxt_ = this->free_list_;
            this->free_list_ = entry;
         }
   
         delete [] this->slots_;
         this->slots_ = newSlots;
      }
      catch (const std::exception & err)
      {
         // Rollback internal state
         this->capacity_idx_  = oldIdx;
         this->capacity_      = oldCap;
         this->slots_         = oldSlots;
         throw;
      }
   }

   void init()
   {
      this->slots_ = new entry_ptr[this->capacity_];
      this->free_list_ = nullptr;

      for (size_t i = 0; i < this->capacity_; ++i)
      {
         entry_ptr entry = new hash_map_entry();
         entry->nxt_ = this->free_list_;
         this->free_list_ = entry;
      }
   }

public:

   explicit hash_map(hash_fn & h) 
   : capacity_(CAPACITY_LIST[0]),
     capacity_idx_(0),
     size_(0),
     hash_(h)
   {
      this->init();
   }
 
   hash_map()
   : capacity_(CAPACITY_LIST[0]),
     capacity_idx_(0),
     size_(0),
     hash_(default_hash())
     //aux_hash_(aux_hash())
   {
      this->init();
   }

   ~hash_map()
   {
      while (this->free_list_ != nullptr)
      {
         entry_ptr tmp = this->free_list_;
         this->free_list_ = this->free_list_->nxt_;
         delete tmp;
      }

      delete [] this->slots_;
   }

   value_type * find(const key_type & key)
   {
      value_type * v = nullptr;
      int offset = -1;

      int i = this->hash_(key) % this->capacity_;
      entry_ptr & e = this->slots_[i];

      for(size_t j = 0; (e != nullptr) && (j < this->capacity_); ++j)
      {
         if (e->key_ == key)
         {
            v = &(e->value_);
            break;
         }
        
         // Execute the aux hash function if we "missed" with our initial
         // key lookup
         if (offset < 0)
         {
            struct aux_hash ah;
            offset = ah(i, (int) this->capacity_);
         }
         i += offset % this->capacity_;
         e = this->slots_[i]; 
      } 

      return v;
   }

   value_type * operator[](const key_type & key)
   {
      return this->find(key);
   }

   bool insert(const key_type & key, const value_type & val)
   {
      bool did_insert = false;
      int offset = -1;

      int i = this->hash_(key) % this->capacity_;
      entry_ptr & e = this->slots_[i];

      for(size_t j = 0; j < this->capacity_; ++j)
      {
         if (e == nullptr)
         {
            e = this->free_list_;
            e->key_   = key;
            e->value_ = val;
            this->free_list_ = this->free_list_->nxt_;
            did_insert = true;
            ++this->size_;
            break;
         }
        
         // Execute the aux hash function if we "missed" with our initial
         // key lookup
         if (offset < 0)
         {
            struct aux_hash ah;
            offset = ah(i, (int) this->capacity_);
         }

         i += offset % this->capacity_;
         e = this->slots_[i]; 
      }

      if (((double) this->size_ / (double) this->capacity_) >= LOAD_FACTOR) 
      {
         this->grow();
      }

      return did_insert;
   }

//   bool erase(const key_type & key)
//   {
//
//      --this->size_;
//      return true;
//   }

   std::size_t size() const
   {
      return this->size_;
   }

   std::size_t capacity() const
   {
      return this->capacity_;
   }

};

}

}

}
#endif // HASHMAP_H

