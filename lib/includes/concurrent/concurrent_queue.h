#ifndef CONCURRENT_QUEUE_H
#define CONCURRENT_QUEUE_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>

namespace sk3l {

namespace concurrent {

template <class T>
class concurrent_queue
{
   private:

      struct cq_node
      {
         T         value_;
         cq_node * next_;
      };
      
      using node_t         = cq_node;
      using node_ptr_t     = cq_node *; 
      using ptr_t          = T *;
      using locker_t       = std::unique_lock<std::mutex>;
      using timed_locker_t = std::unique_lock<std::timed_mutex>; 
  
      node_ptr_t                    head_;
      node_ptr_t                    tail_;
      std::atomic_size_t            size_; 
      std::atomic_bool              tear_down_;
      std::mutex                    head_lock_;
      std::timed_mutex              tail_lock_;
      std::condition_variable_any   queue_cv_;
      
      node_ptr_t get_tail() 
      {
         timed_locker_t gaurd(this->tail_lock_);
         return this->tail_;
      }

      void append_node(node_ptr_t new_tail, const T & val)
      {
         this->tail_->value_ = val;
         this->tail_->next_ = new_tail;
         this->tail_        = new_tail;
      } 

   public:
      concurrent_queue()
         : head_(new cq_node()), 
           tail_(head_),
           size_(0),
           tear_down_(false)
      {}

      // This is non-copyable.
      concurrent_queue(const concurrent_queue & rhs) = delete;
      concurrent_queue & operator=(const concurrent_queue & rhs) = delete;

      void enqueue(const T & t)
      {
         if (this->tear_down_.load())
               return;

         std::unique_ptr<node_t> new_tail(new node_t()); 
         {
            timed_locker_t locker(this->tail_lock_);
            this->append_node(new_tail.release(), t);
         }
         this->size_.fetch_add(1);
         this->queue_cv_.notify_one();
      }

      bool try_enqueue(const T & t, int timeout)
      {
         if (this->tear_down_.load())
               return false;

         std::unique_ptr<node_t> new_tail_(new node_t());

         if (this->tail_lock_.try_lock_for(std::chrono::microseconds(timeout)))
            this->append_node(new_tail_.release(), t);
         else
            return false;
 
         this->size_.fetch_add(1);
         this->queue_cv_.notify_one();
         
         return true;
      }

      T * dequeue()
      {
         if (this->tear_down_.load())
               return nullptr;
         
         T * rv = nullptr;

         locker_t locker(this->head_lock_);
         this->queue_cv_.wait
         (
            locker,
            [this] 
            {return this->tear_down_.load() || this->head_ != this->get_tail();}
         );

         if (this->tear_down_.load())
            return nullptr; 

         rv = &(this->head_->value_);
         this->head_ = this->head_->next_; 
         
         this->size_.fetch_sub(1);
         return rv;
      }

      bool try_dequeue(T * & t, int timeout)
      {
         t = nullptr;
         if (this->tear_down_.load())
               return false;
        
         bool rv = false; 
         locker_t locker(this->head_lock_);
         if
         (
            this->queue_cv_.wait_for
            (
               locker,
               std::chrono::microseconds(timeout),
               [this] 
               {return this->tear_down_.load() ||this->head_ != this->get_tail();}
            )
         )
         {
            if (!this->tear_down_.load())
            {
               t = &(this->head_->value_);
               this->head_ = this->head_->next_;
               this->size_.fetch_sub(1);
               rv = true;
            }
         }

         return rv;
      }

      void release() 
      {
         this->tear_down_.store(true);
         this->queue_cv_.notify_all();
      }

      size_t size() const
      {
         return this->size_.load();
      }

      ~concurrent_queue()
      {
         if (!this->tear_down_.load())
            this->release();

         node_ptr_t np = this->head_;
         do
         {
            this->head_ = np->next_;
            delete np;
            np = this->head_;
         }
         while (np != nullptr);

         this->head_ = this->tail_ = nullptr;

      }
};

}

}

#endif
