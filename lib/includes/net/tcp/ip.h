#ifndef TCP_CONFIG_H
#define TCP_CONFIG_H

#include <cstdint>
#include <exception>
#include <ostream>
#include <string>
#include <string.h>
#include <utility>

#include "core/text/string_util.h"

namespace sk3l {

namespace net {

namespace tcp {

enum ip_version 
{
   NoVersion= -1,
   IPv4     = 0,
   IPv6     = 1
};

struct ip_v4
{
   ip_v4(std::uint16_t port, std::uint32_t octets)
      : port_(port), octets_(octets)
  {}

   std::uint16_t port_;
   std::uint32_t octets_;

   template <typename charT=char, typename traits=std::char_traits<charT> >
   std::basic_string<charT,traits> to_string() const
   {
      std::basic_string<charT,traits> result;
      using string_util = sk3l::core::text::basic_string_util<charT>;
      
      charT dots[4] = {0, '.', '.', '.'};

      int cnt = 3;
      while (cnt >= 0)
      {
         result +=
            string_util::template numeric_to_string<std::uint8_t>
            (
               (this->octets_ >> (8 * cnt)) & 0xff
            );

         result.push_back(dots[cnt--]); 
      }
      result += ":";
      result += string_util::template numeric_to_string<std::uint16_t>(this->port_);


      return result;
   }
};

struct ip_v6
{
   ip_v6
   (
      std::uint16_t port, 
      std::uint32_t flow, 
      const std::uint8_t * octets, 
      std::uint32_t scope
   )
      : port_(port),
        flow_info_(flow),
        octets_(),
        scope_id_(scope)
   {
      memcpy(this->octets_, octets, sizeof(octets_));
   } 

   std::uint16_t port_;
   std::uint32_t flow_info_;
    std::uint8_t octets_[16];
   std::uint32_t scope_id_; 
};

using uint8_t_string_util = sk3l::core::text::basic_string_util<uint8_t>;

class ip_address
{
   private:
      ip_version version_;
      struct ip_v4 * v4_addr_;
      struct ip_v6 * v6_addr_;

   public:
      static const std::size_t V4_OCTET_LEN = 4;
      static const std::size_t V6_OCTET_LEN = 16;

      ip_address()
         : version_(tcp::ip_version::NoVersion),
           v4_addr_(nullptr),
           v6_addr_(nullptr) 
      {}

      ip_address(const ip_address & rhs)
         : version_(rhs.version_),
         v4_addr_
         (
            rhs.v4_addr_ == nullptr ? 
               nullptr : new ip_v4 {rhs.v4_addr_->port_, rhs.v4_addr_->octets_}
         ),
         v6_addr_
         (
            rhs.v6_addr_ == nullptr ? 
               nullptr : new ip_v6 
               (
                  rhs.v6_addr_->port_, 
                  rhs.v6_addr_->flow_info_,
                  rhs.v6_addr_->octets_,
                  rhs.v6_addr_->scope_id_
               )
         )
       {}

      ip_address & operator= (const ip_address & rhs)
      {
         if (&rhs == this)
            return *this;

         this->version_ = rhs.version_;

         if (rhs.version_ == tcp::ip_version::IPv4)
         {
            this->v4_addr_ = new ip_v4{rhs.v4_addr_->port_, rhs.v4_addr_->octets_};
            this->v6_addr_ = nullptr;
         }
         else
         {
            this->v6_addr_ = new ip_v6
            (
               rhs.v6_addr_->port_,
               rhs.v6_addr_->flow_info_,
               rhs.v6_addr_->octets_,
               rhs.v6_addr_->scope_id_
            );
            this->v4_addr_ = nullptr;
         }

         return *this;
      }

      static ip_address make_v4
      (
         std::uint16_t port, 
         std::uint32_t octets
      ) 
      {
         ip_address ipaddr;
         ipaddr.version_ = tcp::ip_version::IPv4;
         ipaddr.v4_addr_ = new ip_v4 {port, octets};
         return ipaddr;
      }

      template<typename cT=char, typename tT=std::char_traits<cT> >
      static ip_address make_v4
      (
         std::uint16_t port, 
         const std::basic_string<cT,tT> & octets
      ) 
      {
         using str_t = std::basic_string<cT,tT>;

         std::uint32_t i_octets = 0;
         size_t pos = 0,
                len = octets.length();
         
         int cnt = 3;
         while (cnt >= 0 && pos < len)
         {
            std::uint8_t octet;
            
            size_t nxt_pos = octets.find('.', pos);
            if (nxt_pos == str_t::npos)
               nxt_pos = len; 

            octet = 
               sk3l::core::text::basic_string_util<cT>::template string_to_numeric<uint8_t>
               (
                  octets.substr(pos, nxt_pos - pos) 
               ); 

            i_octets |= octet << 8 * cnt;

            --cnt;
            pos = nxt_pos + 1;
         }
         
         if (cnt >= 0)
            throw std::invalid_argument
            (
               std::string("Invalid IP string '") + octets + std::string("'.")
            );

         
         return make_v4(port, i_octets);
      }


      static ip_address make_v6
      (
         std::uint16_t  port, 
         std::uint32_t  flow, 
   const std::uint8_t * octets, 
         std::uint32_t  scope_id
      )
      {
         ip_address ipaddr;
         ipaddr.version_ = tcp::ip_version::IPv6;
         ipaddr.v6_addr_ = new ip_v6 {port, flow, octets, scope_id};

         return ipaddr;
      } 

      ip_version  get_version() const  {return this->version_;}
      ip_v4 *     get_v4_addr() const  {return this->v4_addr_;}
      ip_v6 *     get_v6_addr() const  {return this->v6_addr_;} 

      bool        is_valid()    const  {return this->version_ != ip_version::NoVersion;}

      ~ip_address()
      {
         if (this->v4_addr_ != nullptr)
         {
            delete this->v4_addr_;
            this->v4_addr_ = nullptr;
         }
         else if (this->v6_addr_ != nullptr)
         {
            delete this->v6_addr_;
            this->v6_addr_ = nullptr;
         }
      }
};

template<typename charT=char, typename traits=std::char_traits<charT> >
std::basic_ostream<charT,traits>  & operator<< 
(
   std::basic_ostream<charT,traits> & os, 
   const ip_address & ip
)
{
   if (ip.get_version() == ip_version::IPv4)
      os << ip.get_v4_addr()->to_string(); 

   return os; 
}


}
}
}

#endif
