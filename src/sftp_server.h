#ifndef SFTP_SERVER_H
#define SFTP_SERVER_H

#include <memory>
#include <string>

#include <libssh/libssh.h>

namespace charon {

   enum sftp_log_level
   {
      Off   = SSH_LOG_NOLOG,
      Warn  = SSH_LOG_WARNING,
      Info  = SSH_LOG_PROTOCOL,
      Debug = SSH_LOG_PACKET,
      Trace = SSH_LOG_FUNCTIONS
   };

   class sftp_connection;
   using sftp_conn_ptr = std::unique_ptr<sftp_connection>;

   class sftp_server
   {
      private :

         ssh_session session_;
         std::string host_;

         bool authenticate_server();
         bool authenticate_user(const std::string & user);

      public :

         sftp_server(const std::string & host, long port);
         ~sftp_server();

         sftp_conn_ptr connect(const std::string & user);

         bool        is_connected() const;
         std::string get_host() const;
         long        get_port() const;

         inline ssh_session    get_ssh_session() const   {return this->session_;}

         sftp_server(const sftp_server & rhs) = delete;
         sftp_server & operator=(const sftp_server & rhs) = delete;

   };
}

#endif // SFTP_SERVER_H
