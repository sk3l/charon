#include <memory>
#include <sstream>

#include <string.h>

#include <libssh/libsshpp.hpp>


#include "sftp_server.h"




namespace charon {

sftp_server::sftp_server(const std::string & host, long port)
   : 
      session_(ssh_new()),
		host_(host),
		port_(port),
		connected_ (false),
		log_level_(Warn)
{
   ssh_options_set(this->session_, SSH_OPTIONS_HOST, host_.c_str());
   ssh_options_set(this->session_, SSH_OPTIONS_PORT,  &port_);   
}

sftp_server::~sftp_server()
{
   ssh_free(this->session_);
   this->session_ = nullptr;
}

void sftp_server::connect()
{
   // Build the connection
		

	int rc = ssh_connect(this->session_);
  	if (rc != SSH_OK)
  	{
		//std::stringstream err;
 		//err << "Error connecting to " << this->host_
      //    << ssh_get_error(this->session_); 
	
		throw ssh::SshException(this->session_);	
  	}
   

   // Authenticate the server
   //
   this->auth_server();

   // Authenticate the user
   //
   this->auth_user();


   this->connected_ = true;
}

void sftp_server::auth_server()
{
   int rc, state;
   size_t hlen;
   unsigned char *hash = NULL;
   char buf[10];

   bool auth_ok = true;


   ssh_key server_key;
 
   state = ssh_is_server_known(this->session_);
   
   rc = ssh_get_publickey(this->session_, &server_key);
  	if (rc != SSH_OK)
      throw ssh::SshException(this->session_);

   rc = ssh_get_publickey_hash(server_key, SSH_PUBLICKEY_HASH_MD5, &hash, &hlen);
   if (rc != SSH_OK)
       throw ssh::SshException(this->session_);  

   ssh_key_free(server_key);

 	switch (state)
   {
      case SSH_SERVER_KNOWN_OK:
      break;

      case SSH_SERVER_KNOWN_CHANGED:
         fprintf(stderr, "Host key for server changed: it is now:\n");
         ssh_print_hexa("Public key hash", hash, hlen);
         fprintf(stderr, "For security reasons, connection will be stopped\n");
         auth_ok = false;
      break;

      case SSH_SERVER_FOUND_OTHER:
         fprintf(stderr, "The host key for this server was not found but an other"
         "type of key exists.\n");
         fprintf(stderr, "An attacker might change the default server key to"
         "confuse your client into thinking the key does not exist\n");
         auth_ok = false;
      break;

      case SSH_SERVER_FILE_NOT_FOUND:
         fprintf(stderr, "Could not find known host file.\n");
         fprintf(stderr, "If you accept the host key here, the file will be"
            "automatically created.\n");
       // fallback to SSH_SERVER_NOT_KNOWN behavior
     
      case SSH_SERVER_NOT_KNOWN:
      {
         std::unique_ptr<char> hexa(ssh_get_hexa(hash, hlen));
         fprintf(stderr,"The server is unknown. Do you trust the host key?\n");
         fprintf(stderr, "Public key hash: %s\n", hexa.get());
         if (fgets(buf, sizeof(buf), stdin) == NULL)
         {
            auth_ok = false;
            break;
         }

         if (strncasecmp(buf, "yes", 3) != 0)
         {
            auth_ok = false;
            break;
         }

         if (ssh_write_knownhost(this->session_) < 0)
         {
            fprintf(stderr, "Error %s\n", strerror(errno));
            auth_ok = false;
            break;
         }
      }

      case SSH_SERVER_ERROR:
         fprintf(stderr, "Error %s", ssh_get_error(this->session_));
         auth_ok = false;

   } 
  
   free(hash); 
      
   if (!auth_ok)
      throw ssh::SshException(this->session_);
      
}

void sftp_server::auth_user()
{

}

void sftp_server::set_log_verbosity(sftp_log_level level)
{

}

}
