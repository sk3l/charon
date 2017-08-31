#ifndef SFTP_SESSION_H
#define SFTP_SESSION_H

#include <string>

#include <libssh/libssh.h>
#include <libssh/sftp.h>

#include "sftp_directory.h"
#include "sftp_file.h"

namespace charon {

   class sftp_connection;
   using sftp_conn_ptr = std::shared_ptr<sftp_connection>;
   using sftp_dir_ptr = std::shared_ptr<sftp_directory>;

   class sftp_connection
   {
      friend class sftp_server;

      private :
         ::ssh_session     ssh_sess_;
         ::sftp_session    sftp_sess_;
         std::string       cwd_;

         bool authenticate_server();
         bool authenticate_user(const std::string & user);

         sftp_connection(const std::string & user, const std::string & host, short port);

      public :

         sftp_connection(const sftp_connection & rhs) = delete;
         sftp_connection & operator=(const sftp_connection & rhs) = delete;

         ~sftp_connection();

         std::string    canonicalize(const std::string & path);

         void           change_directory(const std::string & path);
         void           print_working_directory() const;

         sftp_directory read_directory(const std::string & path);

         sftp_file      stat(const std::string & path);
         void           put(const std::string & lpath, const std::string & rpath = "");
         void           get(const std::string & rpath, const std::string & lpath = "");
   };
}

#endif // SFTP_SESSION_H
