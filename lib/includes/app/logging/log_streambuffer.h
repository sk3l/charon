#ifndef LOG_STREAMBUFFER_H
#define LOG_STREAMBUFFER_H

#include <assert.h>

#include <cstring>
#include <cstdio>
#include <streambuf>
#include <locale>
#include <string>
#include <memory>
#include <thread>

#include "log_util.h"

namespace sk3l {
namespace app {

template<class charT, class traits = std::char_traits<charT> >
class basic_log_streambuffer : public std::basic_streambuf<charT, traits>
{
public:
   typedef basic_log_streambuffer<charT,traits> buffer_type;
   typedef std::unique_ptr<charT> charT_ptr;

protected:
   charT_ptr buffer_;
   size_t size_;
   log_level logLevel_;

   static const size_t BUFFER_INIT_SIZE = 4096;

   basic_log_streambuffer(size_t capacity = BUFFER_INIT_SIZE, log_level ls = Unknown)
      : buffer_(new charT[capacity]),
        size_(capacity),
        logLevel_(ls)
   {
      this->resetPointers(0, size_);
   }

   basic_log_streambuffer(charT_ptr & buffer, size_t size, log_level ls = Unknown)
      : buffer_(std::move(buffer)),
        size_(size),
        logLevel_(ls)
   {
      this->resetPointers(0, size_);
   }

   basic_log_streambuffer(basic_log_streambuffer&& source)
      : buffer_(std::move(source.buffer_)),
        size_(source.size_),
        logLevel_(source.logLevel_)
   {
      this->resetPointers(0, size_);
   }

   basic_log_streambuffer& operator=(basic_log_streambuffer&& source)
   {
      if (this != &source)
      {
         this->buffer_ = std::move(source);
         this->size_ = source.size_;
         this->logLevel_ = source.logLevel_;

         this->resetPointers(0, size_);
      }

      return *this;
   }

   virtual int flushBuffer(const std::string & preamble) = 0;

   virtual void resetPointers(size_t start, size_t end, size_t ppos = 0)
   {
      assert((end - start) <= size_);

      this->setp(this->buffer_.get() + start, this->buffer_.get() + end);

      if (ppos > 0)
         this->pbump(ppos);
   }

   virtual int overflow(int c)
   {
      if (c != EOF)
      {
         assert(this->pptr() == this->epptr());

         resize();
         *this->pptr() = c;
         this->pbump(1);
      }

      return c;
   }

   virtual std::streamsize xsputn(const charT *__s, std::streamsize __n)
   {
      std::ptrdiff_t count = 0;

      while (count < __n)
      {
         if (size_ < static_cast<size_t>(__n))
         {
            resize(__n);
         }

         assert(this->epptr() >= this->pptr());
         std::ptrdiff_t len = std::min((this->epptr() - this->pptr()), __n - count);

         if (len > 0)
         {
            std::memcpy(this->pptr(), __s + count, len);
            count += len;
            this->pbump(len);
         }
      }

      return count;
   }

   virtual int sync()
   {
      size_t count = length();
      if (count > 0)
      {
         this->flushBuffer(this->buildPreamble());
         this->resetPointers(0, size_);
      }

      return 0;
   }

   virtual std::string buildPreamble()
   {
      std::basic_ostringstream<charT,traits> sstream;

      sstream << "[" << log_util::get_current_date_str() << "]";
      sstream << "[" << std::this_thread::get_id() << "]";
      sstream << "[" << log_util::get_level_str(logLevel_) << "] ";

      return sstream.str();
   }

   inline size_t length()
   {
      return (this->pptr() - this->pbase());
   }

private:
   void resize(size_t size = 0)
   {
      if (size > 0)
         size_ = size;
      else
         size_ *= 2;

      charT_ptr newBuffer(new charT[size_]);
      std::memset(newBuffer.get(), 0, size_);

      size_t len = this->length();
      std::memcpy(newBuffer.get(), this->pbase(), len);
      buffer_ = std::move(newBuffer);
      this->resetPointers(0, size_, len);
   }

public: 

   basic_log_streambuffer(const basic_log_streambuffer& source) = delete;
   basic_log_streambuffer& operator=(basic_log_streambuffer& source) = delete;

   inline log_level get_log_level() const
   {
      return logLevel_;
   }

   inline void set_log_level(log_level ls)
   {
      logLevel_ = ls;
   }

   virtual ~basic_log_streambuffer()
   {
   }
};

typedef basic_log_streambuffer<char> log_streambuffer;
typedef basic_log_streambuffer<wchar_t> wlog_streambuffer;

}
}

#endif // LOG_STREAMBUFFER_H
