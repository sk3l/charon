#ifndef SFTP_SESSION_H
#define SFTP_SESSION_H

#include <string>

#include <libssh/libssh.h>
#include <libssh/sftp.h>

namespace charon {

   class sftp_connection
   {
      private :
         ::ssh_session     ssh_sess_;
         ::sftp_session    sftp_sess_;

      public :

         sftp_connection(::ssh_session sess);
         ~sftp_connection();

         sftp_connection(const sftp_connection & rhs) = delete;
         sftp_connection & operator=(const sftp_connection & rhs) = delete;

   };
}

#endif // SFTP_SESSION_H
