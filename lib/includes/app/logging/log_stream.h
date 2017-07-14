#ifndef LOG_H
#define LOG_H

#include <ostream>
#include <memory>

#include "log_streambuffer.h"

namespace sk3l 
{
namespace app
{

template<class charT, class traits = std::char_traits<charT> >
class basic_log_stream
{
public:
   typedef std::basic_ostream<charT, traits> ostream_type;
   typedef basic_log_streambuffer<charT,traits> buffer_type;
   typedef buffer_type * buffer_ptr_type;

protected:
   buffer_ptr_type buffer_;
   ostream_type ostream_;

   basic_log_stream()
      : buffer_(nullptr),
        ostream_(buffer_)
   {
   }

   explicit basic_log_stream(buffer_ptr_type buffer)
   {
      this->init(buffer);
   }

   void init(buffer_ptr_type buffer)
   {
      this->buffer_ = buffer;
      this->ostream_.rdbuf(buffer);
   }

public:

   inline void set_log_level(log_level ls)
   {
      buffer_->set_log_level(ls);
   }

   inline log_level get_log_level() const
   {
      return buffer_->get_log_level();
   }

   void write(const std::string & message)
   {
      ostream_ << message << std::endl;
   }

   void write(log_level level, const std::string & message)
   {
      this->set_log_level(level);
      this->write(message);
   }

   basic_log_stream& operator<< (bool val)
   {
      return writeValueType(val);
   }

   basic_log_stream& operator<< (short val)
   {
      return writeValueType(val);
   }

   basic_log_stream& operator<< (unsigned short val)
   {
      return writeValueType(val);
   }

   basic_log_stream& operator<< (int val)
   {
      return writeValueType(val);
   }

   basic_log_stream& operator<< (unsigned int val)
   {
      return writeValueType(val);
   }

   basic_log_stream& operator<< (long val)
   {
      return writeValueType(val);
   }

   basic_log_stream& operator<< (unsigned long val)
   {
      return writeValueType(val);
   }

   basic_log_stream& operator<< (long long val)
   {
      return writeValueType(val);
   }

   basic_log_stream& operator<< (unsigned long long val)
   {
      return writeValueType(val);
   }

   basic_log_stream& operator<< (float val)
   {
      return writeValueType(val);
   }

   basic_log_stream& operator<< (double val)
   {
      return writeValueType(val);
   }

   basic_log_stream& operator<< (long double val)
   {
      return writeValueType(val);
   }

   basic_log_stream& operator<< (void* val)
   {
      return writeValueType(val);
   }

   basic_log_stream& operator<<(const charT * t)
   {
      ostream_ << t;
      return *this;
   }

   basic_log_stream& operator<<(const std::string & t)
   {
      ostream_ << t;
      return *this;
   }

   template<class T>
   basic_log_stream& operator<<(const T & t)
   {
      ostream_ << t;
      return *this;
   }

   template<class T>
   basic_log_stream& operator<<(const T * t)
   {
      ostream_ << t;
      return *this;
   }

   typedef std::ostream & (*iomanip)(std::ostream&);

   basic_log_stream& operator<<(iomanip f)
   {
      f(ostream_);
      return *this;
   }

   virtual ~basic_log_stream()
   {}

private:
   template<class T>
   basic_log_stream& writeValueType(T t)
   {
      ostream_ << t;
      return *this;
   }
};

template<class charT, class traits>
basic_log_stream<charT,traits>& operator<<(basic_log_stream<charT,traits>& log, log_level ls)
{
   log.set_log_level(ls);
   return log;
}

typedef basic_log_stream<char> log_stream;
typedef basic_log_stream<wchar_t> wlog_stream;

}
}
#endif // LOG_H
