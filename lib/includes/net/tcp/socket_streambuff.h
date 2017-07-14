#ifndef WRITE_BUFFER_H
#define WRITE_BUFFER_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>

namespace sk3l {

namespace net {

namespace tcp {

template<typename charT=char>
class basic_net_buffer : public std::streambuf
{
   protected:
      socket *    sock_;
      charT  *    buff_;
      std::size_t size_;

      inline size_t length() {return (this->gptr() - this->gbase());}

   private:
      
      void resize(size_t size = 0)
      {
         if (size > 0)
            size_ = size;
         else
            size_ += BLOCK_SIZE;
   
         charT * newbuff(new charT[this->size_]);
         std::memset(newbuff, 0, this->size_);
   
         size_t len = this->length();
         std::memcpy(newbuff, this->pbase(), len);
         buffer_ = std::move(newBuffer);
         this->resetPointers(0, size_, len);
      }



   public:
      //using chunk_t = net::chunk<charT>;
      static const std::size_t BLOCK_SIZE = 4096;

      // Ctors
      basic_net_buffer()
      :
        sock_(nullptr),
        buff_(new charT[BLOCK_SIZE]),
        size_(BLOCK_SIZE)
     {
        charT * end = this->buff_ + BLOCK_SIZE;
        this->setg(end, end, end);
     }

      basic_net_buffer
      (
         socket * sock,
         const charT * data,
         std::size_t size
      )
      :
         socket_(sock),
         buff_(data),
         size_(size) 
      {
         charT * end = this->buff_ + BLOCK_SIZE;
         this->setg(end, end, end);
      }

      basic_net_buffer(const basic_net_buffer & rhs)
      :
        sock_(rhs.sock_),
        buff_(new charT[rhs.size_]),
        size_(rhs.size_)
      {
         std::memcpy(this->buff_, rhs.buff_, rhs.size());
         this->setg
         (
            this->buff_,
            this->buff_ + (rhs.gptr() - rhs.eback()),
            this->buff_ + (rhs.egptr()- rhs.eback())
         ); 
      }

      basic_net_buffer & operator=(const basic_net_buffer & rhs)
      {
         if (&rhs != this)
         {
            this->sock_ = rhs.sock_;
            this->buff_ = new charT[rhs.size()];
            this->size_ = rhs.size__;            
            std::memcpy(this->buff_, rhs.buff_, rhs.size_);
            this->setg
            (
               this->buff_,
               this->buff_ + (rhs.gptr() - rhs.eback()),
               this->buff_ + (rhs.egptr()- rhs.eback())
            ); 
         }
         return *this;
      }

      ~basic_net_buffer()
      {
         this->sock_.close();
         this->sock_ = nullptr;
         delete [] this->buff_;
         this->buff_ = nullptr;
      }
   
   protected:
/*
      virtual void reset_pointers(size_t start, size_t end, size_t ppos = 0)
      {
         assert((end - start) <= size_);

         this->setg(this->buff_ + start, this->buff_ + end);

         if (gpos > 0)
            this->gbump(gpos);
      }
*/
      virtual int_type underflow()
      {
         int_type rv = *this->gptr();
         if (this->gptr() == this->egptr())
         {
            // Out of data; prime the pump
            this->sock_-> 

            this->gbump(1);    
         }

         return rv; 
      }

      virtual int_type xsgetn(charT *__s, std::streamsize __n)
      {

      }

      virtual int sync()
      {

      }

      // Metadata
      inline std::size_t   get_size()          const {return this->end_ - this->beg_;}
      inline std::size_t   get_read_pos()      const {return this->r_pos_;}
      inline std::size_t   get_write_pos()     const {return this->w_pos_;}
      inline bool          is_read_eof()       const {return this->r_pos_ == this->w_pos_;}
      inline bool          is_write_eof()      const {return this->w_pos_ == this->size_;}

      // Accessors
      inline charT* data()   {return this->buff_;}
};

using net_buffer = basic_net_buffer<char>; 
using wnet_buffer = basic_net_buffer<wchar_t>;
using byte_buffer = basic_net_buffer<std::uint8_t>;

}

}

}

#endif
