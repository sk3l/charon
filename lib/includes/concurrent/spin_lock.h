#ifndef SPIN_LOCK_H
#define SPIN_LOCK_H

#include <atomic>

namespace sk3l {
namespace concurrent {

class spin_lock
{
   private:
      std::atomic_flag lock_ = ATOMIC_FLAG_INIT;

   public:

      spin_lock();

      spin_lock(const spin_lock & rhs) = delete;
      spin_lock & operator=(const spin_lock & rhs) = delete;

      void wait();
      bool try_wait();
      void release(); 
};

}
}
#endif
