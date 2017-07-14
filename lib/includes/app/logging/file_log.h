#ifndef FILE_LOG_H
#define FILE_LOG_H

#include "file_log_streambuffer.h"
#include "log_util.h"
#include "log_stream.h"

namespace sk3l {
namespace app {

template<class charT, class traits = std::char_traits<charT> >
class basic_file_log : public basic_log_stream<charT,traits>
{
public:
   typedef basic_file_log_streambuffer<charT,traits> file_buffer_type;
   typedef std::unique_ptr<file_buffer_type> file_buffer_ptr;

private:

   file_buffer_type fileBuffer_;

public:
   basic_file_log()
   {
   }

   basic_file_log(const std::string & fileName)
      : fileBuffer_(fileName)
   {
      this->init(&fileBuffer_);
   }

   basic_file_log(basic_file_log && source)
      : fileBuffer_(std::move(source.fileBuffer_)),
        basic_log_stream<charT,traits>(&fileBuffer_)
   {
   }

   basic_file_log & operator=(basic_file_log && source)
   {
      if (this != &source)
      {
         this->fileBuffer_ = std::move(source.fileBuffer_);
         this->buffer_ = &fileBuffer_;
         this->ostream_.rdbuf(this->buffer_);
      }

      return *this;
   }

   basic_file_log(const basic_file_log & source) = delete;
   basic_file_log & operator=(basic_file_log & source) = delete;
};

typedef basic_file_log<char> file_log;
typedef basic_file_log<wchar_t> wfile_log;

}
}
#endif // FILE_LOG_H
