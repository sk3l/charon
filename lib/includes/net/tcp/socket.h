#ifndef SOCKET_H
#define SOCKET_H

#include <cstdint>

#include "net/io_buffer.h"
#include "ip.h"

namespace sk3l {

namespace net {

namespace tcp {

using socket_handle_t = int;

enum socket_type
{
   NoType   = -1,
   Stream   = 0,
   Datagram = 1
};

enum socket_shutdown
{
   NoShutdown = -1,
   Send       = 0,
   Receive    = 1,
   Both       = 2
};

enum socket_options
{
   NoOptions    = -1,
   NonBlocking  = 0,
   KeepAlive    = 1,
   Linger       = 2,
   ReadBufSize  = 3,
   WriteBufSize = 4,
};

enum io_result_code
{
   Success        = 0,
   WouldBlock     = 1,
   ClosedByPeer   = 2
};

struct io_result
{
   io_result_code code_;
   std::size_t    length_;
};
 

// Interface wrapping a TCP socket
class socket
{
   protected:
      struct ip_address ipaddr_;
                   long options_;

   public:

      explicit socket
      (
         const ip_address & ipa,
         long  socket_options = OPTION_NONE
      )
         : ipaddr_(ipa),
           options_(socket_options)
      {}

      // Connection management methods for clients
      virtual     bool connect() = 0; 

      // Connection management methods for servers 
      virtual     bool listen() = 0;
      virtual socket * accept() = 0; 
      virtual     void shutdown(tcp::socket_shutdown ss) = 0;
      virtual     void close()   = 0;

      // Data management structures codes/structures 
      
      using io_result_t = struct io_result;

      // Global socket configuration options 
      const static int OPTION_NONE          = 0;
      const static int OPTION_NON_BLOCKING  = 1;
      const static int OPTION_KEEP_ALIVE    = 2;
      const static int OPTION_LINGER        = 4;

      // Per recv/send operation configuration options
      const static int IO_OPTION_NONE       = 0;
      const static int IO_OPTION_DONT_BLOCK = 1;
      const static int IO_OPTION_WAIT_ALL   = 2; 
      const static int IO_OPTION_PEEK       = 4;

      // Data management methods 
      //
      using data_t      = std::uint8_t;
      using data_ptr_t  = data_t*;

      // Write data methods
      virtual io_result_t send
      (
         io_buffer::iterator beg,
         io_buffer::iterator end, 
         int send_options = IO_OPTION_NONE
      ) = 0;

      virtual io_result_t send
      (
         data_ptr_t src,
         std::size_t cnt,
         int send_options = IO_OPTION_NONE
      ) = 0;
      
      virtual io_result_t send_all
      (
         io_buffer::iterator beg,
         io_buffer::iterator end, 
         int send_options = IO_OPTION_NONE
      ) = 0;

      virtual io_result_t send_all
      (
         data_ptr_t src,
         std::size_t cnt,
         int send_options = IO_OPTION_NONE
      ) = 0;
 
      //
      // Read data methods
      virtual io_result_t recv 
      (
         io_buffer::iterator dest,
         int recv_options = IO_OPTION_NONE
      ) = 0;
      
      virtual io_result_t recv 
      (
         data_ptr_t dest,
         std::size_t cnt,
         int recv_options = IO_OPTION_NONE
      ) = 0;
      
      virtual io_result_t recv_all
      (
         io_buffer::iterator dest,
         int recv_options = IO_OPTION_NONE
      ) = 0;

      virtual io_result_t recv_all 
      (
         data_ptr_t dest,
         std::size_t cnt,
         int recv_options = IO_OPTION_NONE
      ) = 0;
 
      // Properties management methods      
      virtual bool is_open() const  = 0;

      virtual void set_socket_options(long opts) = 0;
      virtual long get_socket_options()          = 0;

      ip_address   get_ip_address() const
      {
         return this->ipaddr_;
      }

      virtual socket_handle_t get_socket_handle() const = 0;

      virtual ~socket()
      {}

};

}

}

}

#endif
