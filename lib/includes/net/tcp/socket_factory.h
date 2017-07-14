#ifndef SOCKET_FACTORY_H
#define SOCKET_FACTORY_H

//#include "tcp_config.h"

namespace sk3l {

namespace net {

namespace tcp {

   class ip_address;
   class socket;

   template<class charT>
   class basic_socket_manager;

   // Utility for creating handle to platform-agnostic TCP socket.
   class socket_factory
   {
      public:

         static socket * create_socket(const ip_address & ipa);         
         static socket_manager * create_socket_manager(const ip_address & ipa);        

   };



}
}
}

#endif
