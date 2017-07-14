#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <future>
#include <memory>
#include <thread>
#include <type_traits>
#include <vector>

#include "concurrent_queue.h"

namespace sk3l {

namespace concurrent {

class thread_pool_job_base
{
   private:

   public:
      virtual void run() = 0;
      virtual ~thread_pool_job_base() {}  
};

// Job returning 
template <class RetVal>
class thread_pool_job_rv : public thread_pool_job_base
{
   public:

   protected:
   
      std::function<RetVal()> task_; 

   public:

      thread_pool_job_rv(std::function<RetVal()> && f)
         : task_(std::move(f))
      {}

      void run()
      {
         this->task_();
      }

      void operator()()
      {
         this->run();
      }

       ~thread_pool_job_rv()
      {}
};

class thread_pool
{
   private:

      using job_t          = thread_pool_job_base;
      using job_ptr_t      = std::shared_ptr<job_t>; 
      using thread_t       = std::shared_ptr<std::thread>;
      using thread_pool_t  = std::vector<thread_t>;
      using thread_queue_t = concurrent_queue<job_ptr_t>;

      thread_pool_t  work_threads_;
      thread_queue_t work_queue_;

      std::atomic_bool stop_flag_;

      void start_workers(size_t cnt);
      void work_routine();

   public:

      thread_pool();
      explicit thread_pool(size_t thread_cnt);

      // Non-copyable
      thread_pool(const thread_pool & rhs)            = delete;
      thread_pool & operator=(const thread_pool & rhs)= delete;

      // Post job on queue, don't wait for result
      void post(const job_ptr_t & job);

      // Post job on queue, wait for result
      template <typename Fn>
      std::future<typename std::result_of<Fn>::type> post
      (
         Fn fn
      )
      {
         using return_t = typename std::result_of<Fn()>::type;

         std::packaged_task<return_t> task(std::move(fn));
         std::future<return_t> ret_val(task.get_future());

         this->work_queue_.enqueue(std::move(task));

         return ret_val;
      } 

      void shutdown();

      ~thread_pool()
      {}
};

}

}

#endif
