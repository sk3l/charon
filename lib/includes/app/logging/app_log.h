#ifndef APP_LOG_H
#define APP_LOG_H

#include <memory>
#include <mutex>
#include <vector>

#include "log_stream.h"
#include "log_util.h"

namespace sk3l {
namespace app {

template<class charT, class traits = std::char_traits<charT> >
class basic_app_log
{
public:
   typedef basic_log_stream<charT, traits>   log_type;
   typedef std::unique_ptr<log_type>         log_ptr;

private:
   std::mutex lock_;

   std::vector<log_ptr> logList_;

public:
   basic_app_log()
   {
   }

   void take(log_ptr log)
   {
      std::lock_guard<std::mutex> lock(this->lock_);
      this->logList_.push_back(std::move(log));
   }

   void write(log_level level, const std::string & str)
   {
      std::lock_guard<std::mutex> lock(this->lock_);
      for (auto i = this->logList_.begin(); i != this->logList_.end(); ++i)
         i->get()->write(level, str);
   }
};

typedef basic_app_log<char> log;
typedef basic_app_log<wchar_t> wlog;

}
}

#endif // APP_LOG_H
