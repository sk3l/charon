#ifndef SOCKET_LNX_H
#define SOCKET_LNX_H

// Linux-only platform code
#ifdef  __linux__

#include "net/io_buffer.h"
#include "net/tcp/ip.h"
#include "net/tcp/socket.h"

namespace sk3l {

namespace net  {

namespace tcp  {

class socket_lnx : public socket
{
   private:
      int    fd_;     // file descriptor
      size_t backlog_size_;
      bool   is_connected_;

      using base_t = socket; 

      int xlate_io_options(int options);

   public:

      static const size_t DEFAULT_BACKLOG = 100;

      static const int SOCK_SUCCESS   = 0;
      static const int INVALID_SOCKET = -1;

      explicit socket_lnx
      (
         const ip_address & ipa, 
         size_t backlog = DEFAULT_BACKLOG
      );

      // Connection management methods
          bool connect()    override; 

          bool listen() override;
      socket * accept() override;

      void shutdown(tcp::socket_shutdown ss) override;
      void close() override;
 
      // Data management methods 
      
      // 
      // Write data methods
      virtual io_result send
      (
         io_buffer::iterator beg,
         io_buffer::iterator end, 
         int send_options = IO_OPTION_NONE
      ) override;
 
      virtual io_result_t send
      (
         data_ptr_t src,
         std::size_t cnt,
         int send_options = IO_OPTION_NONE
      ) override;
      
      virtual io_result send_all
      (
         io_buffer::iterator beg,
         io_buffer::iterator end, 
         int send_options = IO_OPTION_NONE
      ) override;

      virtual io_result_t send_all
      (
         data_ptr_t src,
         std::size_t cnt,
         int send_options = IO_OPTION_NONE
      ) override;
 
      //
      // Read data methods
      virtual io_result recv
      (
         io_buffer::iterator dest,
         int recv_options = IO_OPTION_NONE
      ) override;
 
      virtual io_result_t recv 
      (
         data_ptr_t dest,
         std::size_t cnt,
         int recv_options = IO_OPTION_NONE
      ) override;
      
      virtual io_result recv_all
      (
         io_buffer::iterator dest,
         int recv_options = IO_OPTION_NONE
      ) override;
     
      virtual io_result_t recv_all 
      (
         data_ptr_t dest,
         std::size_t cnt,
         int recv_options = IO_OPTION_NONE
      ) override;
 
      // Properties management methods      
      bool is_open() const override;
 
      virtual void set_socket_options(long opts) override;
      virtual long get_socket_options() override;

      inline socket_handle_t get_socket_handle() const override
      {
         return this->fd_;
      }
      
      ~socket_lnx() override;
};

}
}
}

#endif

#endif
