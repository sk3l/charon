#ifndef CONSOLE_STREAMBUFFER_H
#define CONSOLE_STREAMBUFFER_H

#include <iostream>

#include "log_streambuffer.h"

namespace sk3l {
namespace app {

template<class charT, class traits = std::char_traits<charT> >
class basic_console_log_streambuffer : public basic_log_streambuffer<charT,traits>
{

   typedef basic_log_streambuffer<charT,traits> base;

public:

   basic_console_log_streambuffer
   (
      size_t size = basic_log_streambuffer<charT,traits>::BUFFER_INIT_SIZE,
      log_level ls = Unknown
   )
   : log_streambuffer(size, ls)
   {
      std::cout.rdbuf()->pubsetbuf(0,0);
   }

   basic_console_log_streambuffer(basic_console_log_streambuffer && source)
      : base(source)
   {}

   basic_console_log_streambuffer & operator =(basic_console_log_streambuffer && source)
   {
      base::operator =(source);

      return *this;
   }

   ~basic_console_log_streambuffer()
   {
      this->sync();
   }

protected:
   virtual int flushBuffer(const std::string & preamble)
   {
      size_t count = 0;

      if (preamble.length())
      {
         std::cout << preamble;
         count = preamble.length();
      }

      size_t length = this->length();
      std::cout.write(this->buffer_.get(), length);
      this->pbump(length);

      count += length;
      return count;
   }
};

typedef basic_console_log_streambuffer<char> console_log_streambuffer;
typedef basic_console_log_streambuffer<wchar_t> wconsole_log_streambuffer;

}
}

#endif // CONSOLE_STREAMBUFFER_H
