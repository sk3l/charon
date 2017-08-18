#ifndef SFTP_SERVER_H
#define SFTP_SERVER_H

#include <memory>
#include <string>

namespace charon {
/*
   enum sftp_log_level
   {
      Off   = SSH_LOG_NOLOG,
      Warn  = SSH_LOG_WARNING,
      Info  = SSH_LOG_PROTOCOL,
      Debug = SSH_LOG_PACKET,
      Trace = SSH_LOG_FUNCTIONS
   };
*/
   class sftp_connection;
   using sftp_conn_ptr = std::shared_ptr<sftp_connection>;

   class sftp_server
   {
      private :

         std::string host_;
         short       port_;

      public :

         sftp_server(const std::string & host, short port);
         sftp_server(const sftp_server & rhs) = delete;
         sftp_server & operator=(const sftp_server & rhs) = delete;

         ~sftp_server() {};

         sftp_conn_ptr connect(const std::string & user);

         bool        is_connected() const;
         std::string get_host() const {return this->host_;}
         short       get_port() const {return this->port_;}

   };
}

#endif // SFTP_SERVER_H
