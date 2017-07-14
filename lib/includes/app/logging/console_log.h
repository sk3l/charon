#ifndef CONSOLE_LOG_H
#define CONSOLE_LOG_H

#include "console_log_streambuffer.h"
#include "log_util.h"
#include "log_stream.h"

namespace sk3l {
namespace app {

template<class charT, class traits = std::char_traits<charT> >
class basic_console_log : public basic_log_stream<charT,traits>
{
public:
   typedef basic_console_log_streambuffer<charT,traits> console_buffer_type;
   typedef std::unique_ptr<console_buffer_type> console_buffer_ptr;

private:

   console_buffer_type consoleBuffer_;

public:

   basic_console_log()
   {
      this->init(&consoleBuffer_);
   }

   basic_console_log(basic_console_log && source)
      : consoleBuffer_(std::move(source.consoleBuffer_)),
        basic_log_stream<charT,traits>(&consoleBuffer_)
   {
   }

   basic_console_log & operator=(basic_console_log && source)
   {
      if (this != &source)
      {
         this->consoleBuffer_ = std::move(source.consoleBuffer_);
         this->buffer_ = &consoleBuffer_;
         this->ostream_.rdbuf(this->buffer_);
      }

      return *this;
   }

   basic_console_log(const basic_console_log & source) = delete;
   basic_console_log & operator=(basic_console_log & source) = delete;
};

typedef basic_console_log<char> console_log;
typedef basic_console_log<wchar_t> wconsole_log;

}
}

#endif // CONSOLE_LOG_H
