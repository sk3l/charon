#ifndef SFTP_SESSION_H
#define SFTP_SESSION_H

#include <string>

#include <libssh/sftp.h>

namespace charon {

   class sftp_server;

   class sftp_session
   {
      private :
         sftp_server *     server_;          
         ::sftp_session 	session_;

      public :

         sftp_session(sftp_server * server);
         ~sftp_session();

         sftp_session(const sftp_session & rhs) = delete;
         sftp_session & operator=(const sftp_session & rhs) = delete;

   };
}

#endif // SFTP_SESSION_H
