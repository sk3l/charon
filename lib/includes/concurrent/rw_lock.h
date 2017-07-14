#ifndef RW_LOCK_H
#define RW_LOCK_H

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace sk3l {

namespace concurrent {

class rw_lock
{
   private:
      std::mutex              lock_;
      std::condition_variable turnstile_;
      int                     r_counter_;
      bool                    w_flag_;
      std::atomic_bool        tear_down_;

      using scope_lock = std::unique_lock<std::mutex>;

   public:
      rw_lock();

      rw_lock(const rw_lock & rhs)  = delete;
      rw_lock & operator=(const rw_lock & rgs)= delete;

      void enter_read();
      bool try_enter_read();
      void exit_read();

      void enter_write();
      bool try_enter_write();
      void exit_write();

      ~rw_lock();
};

}

}

#endif
