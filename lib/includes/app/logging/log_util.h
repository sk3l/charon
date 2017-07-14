#ifndef log_util_H
#define log_util_H

#include <iosfwd>
#include <string>

#include "core/datetime/date_time.h"
namespace sk3l 
{
namespace app
{

enum log_level
{
   Unknown  = 0,
   Error    = 1,
   Warn     = 2,
   Info     = 3,
   Debug    = 4,
   Trace    = 5
};

class log_util
{
private:
   static const char * const log_level_str_ [6];

public:

   static const std::string get_level_str(log_level ls);

   static const std::string get_current_date_str()
   {
     return sk3l::core::datetime::date_time::now().format_str("%FT%T");
   }

   static const std::wstring get_current_date_wstr()
   {
     return sk3l::core::datetime::date_time::now().format_wstr(L"%FT%T");
   }
};

template<class charT, class traits>
std::ostream& operator<<(std::ostream& os, log_level ls)
{
   os << log_util::get_level_str(ls);
   return os;
}

}
}
#endif // log_util_H
