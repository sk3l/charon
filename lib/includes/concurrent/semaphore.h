#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace sk3l {
namespace concurrent {

class semaphore 
{
   private:
      std::mutex                    lock_;
      std::condition_variable_any   turnstile_;
      int                           count_;
      std::atomic_bool              tear_down_;

      static const int COUNT = 4;

      using scope_lock = std::unique_lock<std::mutex>;

   public:
      semaphore();
      explicit semaphore(int count);

      semaphore(const semaphore & rhs) = delete;
      semaphore & operator=(const semaphore & rhs) = delete;

      void wait();
      bool try_wait();
      void release();

      ~semaphore();
};

}
}

#endif
