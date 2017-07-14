#ifndef IO_BUFFER_H
#define IO_BUFFER_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>

namespace sk3l {

namespace net { 

class io_buffer
{
   private:
      using data_t   = std::uint8_t;
      using data_ptr = data_t *;

      data_ptr       bytes_;
      std::size_t    capacity_;
      std::size_t    size_;

      static const int DEFAULT_CAPACITY = 4096;
   public:

      using value_type = std::uint8_t;

      io_buffer();
      explicit io_buffer(std::size_t size);
      io_buffer(std::uint8_t * & data, const std::size_t & len);
      io_buffer(std::uint8_t * && data, std::size_t && len);
      io_buffer(const io_buffer & rhs);
      io_buffer & operator=(const io_buffer & rhs);
   
      ~io_buffer();
      
      inline std::uint8_t * data() {return this->bytes_;}

      std::size_t capacity() const;
      std::size_t size() const;
      void resize(std::size_t s);
      void clear();

      std::uint8_t & operator[](std::size_t idx);
      std::uint8_t & at(std::size_t idx);

      class iterator
      {
         friend class io_buffer;
         
         public:
         
         using self_type      = iterator;
         using value_type     = data_t;
         using reference_type = data_t&;
         using pointer_type   = data_t*;
         using difference_type= int;
         
         iterator()
            : ptr_(nullptr),
              buff_(nullptr)
         {}
         
         iterator(const iterator & rhs)
         : ptr_(rhs.ptr_),
           buff_(rhs.buff_)
         {
         }
   
         explicit iterator(data_ptr p, io_buffer * iob = nullptr)
            : ptr_(p),
              buff_(iob)
         {
         }
         
         iterator & operator=(const iterator & rhs)
         {
            this->ptr_  = rhs.ptr_;
            this->buff_= rhs.buff_;  
            return *this;
         };
         
         iterator & operator++()
         {
            this->ptr_++;
            return *this;
         } 
         
         iterator operator++(int ignore)
         {
            self_type copy(*this);
            this->ptr_ += 1;
            return copy;
         }
         
         iterator operator--()
         {
            this->ptr_--;
            return *this;
         }
         
         iterator operator--(int ignore)
         {
            self_type copy(*this);
            this->ptr_ -= 1;
            return copy; 
         }
        
         data_t operator[](int idx)
         {
            return this->ptr_[idx];
         }
   
         reference_type operator*()
         {
           return *this->ptr_;
         }

         pointer_type operator&()
         {
            return this->ptr_;
         }

         pointer_type operator->()
         {
            return this->ptr_;
         } 
        
         iterator & operator+=(const int offset)
         {
            this->ptr_ += offset;
            return *this;
         }
   
         iterator & operator-=(const int offset)
         {
            this->ptr_ -= offset;
            return *this;
         }
   
         std::ptrdiff_t operator-(const iterator & rhs) const
         {
            return this->ptr_ - rhs.ptr_;
         }
   
         bool operator==(const iterator & rhs) const
         {
            return this->buff_ == rhs.buff_ && this->ptr_ == rhs.ptr_;
         }
         
         bool operator!=(const iterator & rhs) const
         {
            return !(*this == rhs);
         }
         
#ifdef __DEBUG__
         void print_debug()
         {
            ptr_->print_debug();
         }
#endif
        
         io_buffer * container() 
         {
            return this->buff_;
         }

         private:
         
            data_ptr    ptr_;
            io_buffer * buff_; 
      };
 
      void push_back(std::uint8_t byte);

      iterator insert(iterator pos, std::uint8_t byte);

      void write(iterator beg, iterator end, std::uint8_t * dest);
      
      template<class charT=char, class charTraits=std::char_traits<charT> >
      void write
      (
         iterator beg,
         iterator end,
         std::basic_ostream<charT,charTraits> & dest
      )
      {
         dest.write((charT*)&beg, end - beg);
         this->size_ = 0;
      }
      
      void read(std::uint8_t * src, std::size_t cnt, iterator dest);

      template<class charT=char, class charTraits=std::char_traits<charT> >
      void read 
      (
         std::basic_istream<charT,charTraits> & src,
         std::size_t cnt,
         iterator dest
      )
      {
         for (std::size_t i = 0; src && (i < cnt); ++i)
         {
            charT byte = src.get();
            if (byte == EOF)
              break; 
               
            dest = this->insert(dest, byte);
            dest += 1;
         }  
      }

      iterator begin();
      const iterator cbegin() const;

      iterator end();
      const iterator cend() const;

   private:
      void grow(std::size_t newSize, std::size_t newBeg = 0);
};

net::io_buffer::iterator operator+(net::io_buffer::iterator lhs, std::size_t offset);
net::io_buffer::iterator operator-(net::io_buffer::iterator lhs, std::size_t offset);

bool operator<(const net::io_buffer::iterator & lhs, const net::io_buffer::iterator & rhs);
bool operator>(const net::io_buffer::iterator & lhs, const net::io_buffer::iterator & rhs);
bool operator>=(const net::io_buffer::iterator & lhs, const net::io_buffer::iterator & rhs);
bool operator<=(const net::io_buffer::iterator & lhs, const net::io_buffer::iterator & rhs);

template <class charT=char, class charTraits=std::char_traits<charT> >
std::ostream & operator<<(std::ostream & os, io_buffer & buff)
{

   os.write((charT*)buff.data(), buff.size());
   buff.resize(0);
   return os;
}

template <class charT=char, class charTraits=std::char_traits<charT> >
std::istream & operator>>(std::istream & is, io_buffer & buff)
{
   buff.read(is, std::numeric_limits<std::size_t>::max(),  buff.end());
   return is;
}

}

}

namespace std {
template<> 
struct iterator_traits<sk3l::net::io_buffer::iterator>
{
   typedef std::random_access_iterator_tag   iterator_category;
   typedef sk3l::net::io_buffer::iterator    self_type;
   typedef std::uint8_t                      value_type;
   typedef std::uint8_t &                    reference;
   typedef std::uint8_t *                    pointer;
   typedef int                               difference_type; 
 
};
}

#endif
