#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include <functional>

#include "data/adt/hash_map_c.h"
#include "net/io_buffer.h"
#include "ip.h"
#include "socket.h"

namespace sk3l {

namespace net {

namespace tcp {

class socket_manager 
{
   public:
      using socket_id_t = int;

      struct rw_result
      {
         io_buffer * buff_;
         io_result result_;
      };

      using client_connect_callback = 
         std::function<void(socket_manager &, socket_id_t, ip_address)>;
 
      using socket_disconnect_callback = 
         std::function<void(socket_manager &, socket_id_t, ip_address)>;
      
      using socket_read_callback    = 
         std::function<void(socket_manager &, socket_id_t, rw_result &)>;
   
      using socket_write_callback   =
         std::function<void(socket_manager &, socket_id_t, rw_result &)>;

   protected:
      socket * listen_socket_;

      struct socket_context
      {
         socket_id_t                id_;
         socket *                   socket_;
         socket_disconnect_callback disconnect_callback_;
         socket_read_callback       read_callback_;
         socket_write_callback      write_callback_;
         io_buffer                  buffer_;

         socket_context()
            : id_(0),
              socket_(nullptr),
              read_callback_(),
              write_callback_(),
              buffer_(4096)
         {}
      };

      sk3l::data::adt::hash_map<socket_id_t,socket_context> context_map_; 

   public:

      explicit socket_manager(const ip_address & ip)
      : listen_socket_(nullptr),
        context_map_()
      {}

      virtual void start() = 0;
      
      virtual bool register_connect_handler
      (
         client_connect_callback callback
      ) = 0;
 
      virtual bool register_disconnect_handler
      (
         socket_id_t id,
         socket_disconnect_callback callback
      ) = 0;
      
      virtual bool register_read_handler
      (
         socket_id_t id,
         socket_read_callback callback
      ) = 0;
            
      virtual bool register_write_handler
      (
         socket_id_t id,
         socket_write_callback callback
      ) = 0;

      virtual bool register_read_write_handlers
      (
         socket_id_t id,
         socket_read_callback  r_callback,
         socket_write_callback w_callback
      )
      {
         this->register_read_handler(id, r_callback);
         this->register_write_handler(id,w_callback);
         return true;
      }

/* 
      virtual bool read_from_socket
      (
         socket_id_t sid,
         basic_net_buffer<charT> & buff
      ) = 0;

      virtual bool write_to_socket
      (
         socket_id_t sid,
         basic_net_buffer<charT> & buff
      ) = 0;
*/
      virtual void close() = 0;

      virtual ~socket_manager()
      {}

};

}

}

}

#endif

