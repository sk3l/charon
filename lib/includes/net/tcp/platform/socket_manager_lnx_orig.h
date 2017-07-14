#ifndef SOCKET_MANAGER_LNX_H
#define SOCKET_MANAGER_LNX_H

// Linux-only platform code
#ifdef  __linux__

#include <sys/epoll.h>

#include "net/tcp/socket_manager.h"

namespace sk3l {

namespace net {

namespace tcp {

template<class charT=char>
class basic_socket_manager_lnx : public basic_socket_manager<charT>
{
   public:
      using socket_id_t = typename basic_socket_manager<charT>::socket_id_t;
      
      using conn_callback_t = 
         typename basic_socket_manager<charT>::client_connect_callback;
      using read_callback_t = 
         typename basic_socket_manager<charT>::socket_read_callback;
      using write_callback_t =
         typename basic_socket_manager<charT>::socket_write_callback;

   private:
      static const int MAX_EPOLL_CNT = 1024;

      int efd_;   // epoll file descriptor.

      conn_callback_t   conn_callback_;
      //read_callback_t   read_callback_;
      //write_callback_t  write_callback_;

      void cleanup();
      int change_epoll_flags(int socket_handle, int flags);

   public:
      explicit basic_socket_manager_lnx(const ip_address & ip);

      virtual bool register_connect_handler
      (
         const conn_callback_t & callback
      ) override;
      
      virtual bool register_read_handler
      (
         socket_id_t id,
         read_callback_t callback
      ) override;
            
      virtual bool register_write_handler
      (
         socket_id_t id,
         write_callback_t callback
      ) override;

      virtual void start() override;
/*
      virtual bool read_from_socket
      (
         socket_id_t sid,
         basic_net_buffer<charT> & buff
      ) override;

      virtual bool write_to_socket
      (
         socket_id_t sid,
         basic_net_buffer<charT> & buff
      ) override;
*/
      virtual void close() override;

      virtual ~basic_socket_manager_lnx() override
      {
         this->close();
      }
 
};

using  socket_manager_lnx = basic_socket_manager_lnx<char>;
using wsocket_manager_lnx = basic_socket_manager_lnx<wchar_t>;


}

}

}
#endif

#endif


