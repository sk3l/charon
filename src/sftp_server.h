#ifndef SFTP_SERVER_H
#define SFTP_SERVER_H

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

   class sftp_server
   {
      private :
         
         ssh_session 	session_;
			std::string		host_;
			long				port_;
         bool 				connected_;
         sftp_log_level log_level_;

         void auth_server();
         void auth_user();

      public :

         sftp_server(const std::string & host, long port);
         ~sftp_server();

         void connect();

         void  set_log_verbosity(sftp_log_level level);
         
         inline sftp_log_level get_log_verbosity() const {return this->log_level_;}
         inline bool           is_connected() {return this->connected_;}

         sftp_server(const sftp_server & rhs) = delete;
         sftp_server & operator=(const sftp_server & rhs) = delete;

   };
}

#endif // SFTP_SERVER_H
