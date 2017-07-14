#ifndef LONG_CLOCK_H
#define LONG_CLOCK_H

#include <ctime>
#include <chrono>
#include <cstdint>
#include <memory>
#include <ostream>

namespace sk3l{

namespace core{

namespace datetime {

struct long_clock 
{
   private:
      // The number of seconds between start of long_clock epoch start
      // and default epoch start, i.e. UNIX time start (1.1.1970)
      static const std::uint64_t EPOCH_OFFSET = 62167219200;

      using sys_clock_t = std::chrono::system_clock;
      using sys_time_pt = std::chrono::time_point<std::chrono::system_clock>;

   public:

   using duration    = std::chrono::milliseconds;
   using period      = std::milli;
   using rep         = long int; 
   using time_point  = std::chrono::time_point<long_clock>;
   using timepoint_t = sk3l::core::datetime::long_clock::time_point;
   
   bool is_steady    = false;

   static time_point now()
   {
      return timepoint_t
      (
         std::chrono::duration_cast<duration>
         (
            std::chrono::seconds(EPOCH_OFFSET)
         ) +
         std::chrono::duration_cast<duration>
         (
            sys_clock_t::now().time_since_epoch()
         )
      );     
  }

   static time_point unix_start()
   {
      return time_point
      (
         std::chrono::duration_cast<duration>
         (
            std::chrono::seconds(EPOCH_OFFSET)
         )
      );
   }

   static std::time_t to_time_t(const time_point & tp)
   {
      // First convert tp into conventional UNIX time.
      duration diff = tp - unix_start();
      sys_time_pt stp(diff);
      return sys_clock_t::to_time_t(stp); 
   }

   static time_point from_time_t(const std::time_t & tt)
   {
      sys_time_pt ttime = std::chrono::system_clock::from_time_t(tt);
      sys_time_pt ubeg = sys_time_pt();
      
      duration diff = std::chrono::duration_cast<duration>(ttime - ubeg);

      return time_point(unix_start() + diff);
   }
};


}
}
}

#endif // LONG_CLOCK_H
