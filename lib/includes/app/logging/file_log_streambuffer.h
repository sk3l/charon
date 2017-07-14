#ifndef FILE_LOG_STREAMBUFFER_H
#define FILE_LOG_STREAMBUFFER_H

#include <cstdio>
#include <sstream>

#include "log_streambuffer.h"

namespace sk3l {
namespace app {

template<class charT, class traits = std::char_traits<charT> >
class basic_file_log_streambuffer : public basic_log_streambuffer<charT,traits>
{
   typedef basic_log_streambuffer<charT,traits> base;

private:
   std::FILE * file_;

public:

   basic_file_log_streambuffer()
   {
      file_ = std::fopen(log_util::get_current_date_str().c_str(), "a");
      std::setbuf(file_, nullptr);
   }

   basic_file_log_streambuffer
   (
      const std::string & fileName,
      size_t size = basic_log_streambuffer<charT,traits>::BUFFER_INIT_SIZE,
      log_level ls = Unknown
   )
      : log_streambuffer(size, ls)

   {
      file_ = std::fopen(fileName.c_str(), "a");
      std::setbuf(file_, nullptr);
   }

   basic_file_log_streambuffer(basic_file_log_streambuffer && source)
      : base(source)
   {
      std::fclose(source.file_);
   }

   basic_file_log_streambuffer & operator =(basic_file_log_streambuffer && source)
   {
      base::operator =(source);
      std::fclose(source.file_);

      return *this;
   }

   ~basic_file_log_streambuffer()
   {
      this->sync();
      std::fclose(this->file_);
   }

protected:
   virtual int flushBuffer(const std::string & preamble)
   {
      size_t count = 0;

      if (preamble.length())
      {
         std::fwrite(preamble.c_str(), sizeof(charT), preamble.length(), file_);
         count = preamble.length();
      }

      size_t length = this->length();
      std::fwrite(this->buffer_.get(), sizeof(charT), length, file_);
      this->pbump(length);

      count += length;
      return count;
   }
};

typedef basic_file_log_streambuffer<char> file_log_streambuffer;
typedef basic_file_log_streambuffer<wchar_t> wfile_log_streambuffer;

}
}

#endif // FILE_LOG_STREAMBUFFER_H
