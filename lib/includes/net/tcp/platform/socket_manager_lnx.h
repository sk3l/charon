#ifndef SOCKET_MANAGER_LNX_H
#define SOCKET_MANAGER_LNX_H

// Linux-only platform code
#ifdef  __linux__

#include <cassert>
#include <errno.h>
#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "core/text/string_util.h"
#include "socket_lnx.h"
#include "socket_manager_lnx.h"

#include "net/tcp/socket_manager.h"

namespace sk3l {

namespace net {

namespace tcp {

class socket_manager_lnx : public socket_manager
{
   public:
      using socket_id_t = socket_manager::socket_id_t;

      using conn_callback_t = 
         typename socket_manager::client_connect_callback;
      using disconn_callback_t = 
         typename socket_manager::socket_disconnect_callback;
      using read_callback_t = 
         typename socket_manager::socket_read_callback;
      using write_callback_t =
         typename socket_manager::socket_write_callback;

   private:
      using conn_fp_t = void (*) (socket_manager&,int,ip_address);
      using disconn_fp_t = void (*) (socket_manager&,int,ip_address);
      using read_write_fp_t = void (*) (socket_manager&, int, rw_result&);

      using context_t = typename socket_manager::socket_context;
      
      static const int MAX_EPOLL_CNT = 1024;

      int efd_;   // epoll file descriptor.

      conn_callback_t   conn_callback_;
     
      // Internal helper method for changing the epoll flags
      // assigned to a given file descriptor. 
      int change_epoll_flags
      (
         int socket_handle,
         int flags
      );

      int add_socket_to_epoll
      (
         int socket_handle,
         int flags
      );

      void close_socket(context_t * ctxt); 

      // Internal helper method for cleaning up connections.
      // TO DO: need to process all clients in conn_map_
      void cleanup();

   public:

      explicit socket_manager_lnx(const ip_address & ip);
  
      // Main manager work loop
      virtual void start() override;

      virtual bool register_connect_handler
      (
         conn_callback_t callback
      ) override;

      virtual bool register_disconnect_handler
      (
         socket_id_t id,
         disconn_callback_t callback
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

      virtual void close() override;
    
      virtual ~socket_manager_lnx() override;
};


}

}

}
#endif

#endif


