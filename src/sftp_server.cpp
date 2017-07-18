#include <iostream>
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

bool sftp_server::connect(const std::string & user)
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
   // Authenticate the user
   //
   this->connected_ =
      (this->authenticate_server() && this->authenticate_user(user));

   return this->connected_;
}

bool sftp_server::authenticate_server()
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
         std::cerr << "Host key for server changed! It is now:"
                   << std::endl;
         ssh_print_hexa("Public key hash", hash, hlen);
         std::cerr << "For security reasons, connection will be stopped"
                   << std::endl;
         auth_ok = false;
      break;

      case SSH_SERVER_FOUND_OTHER:
         std::cerr << "The host key for this server was not found but"
                   << " an other type of key exists."
                   << std::endl;
         std::cerr << "An attacker might change the default server key to "
                   << "convince your client the key does not exist"
                   << std::endl;
         auth_ok = false;
      break;

      case SSH_SERVER_FILE_NOT_FOUND:
         std::cerr << "Could not find known host file." << std::endl;
         std::cerr << "If you accept the host key here, the file will be"
                   << "automatically created."
                   << std::endl;
       // fallback to SSH_SERVER_NOT_KNOWN behavior

      case SSH_SERVER_NOT_KNOWN:
      {
         std::unique_ptr<char> hexa(ssh_get_hexa(hash, hlen));

         std::cerr << "The server is unknown. Do you trust the host key?"
                   << std::endl;
         std::cerr << "Public key hash: " << hexa.get() << std::endl;
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
            std::cerr << "Error " << strerror(errno) << std::endl;
            auth_ok = false;
            break;
         }
      }

      case SSH_SERVER_ERROR:
         std:: cerr << "Error " << ssh_get_error(this->session_) << std::endl;
         throw ssh::SshException(this->session_);

   }

   free(hash);
   return auth_ok;

}

bool sftp_server::authenticate_user(const std::string & user)
{
	int rc;

   // First try anonymous access (auth = 'none')
   rc = ssh_userauth_none(this->session_, user.c_str());
 	if (rc == SSH_AUTH_ERROR)
   {
      std::cerr << "Authentication failed: " <<	ssh_get_error(this->session_)
		    		 << std::endl;
  		throw ssh::SshException(this->session_);
	}
	else if (rc == SSH_AUTH_SUCCESS)
   {
      return true;
   }
   else
   {
      // Now let the server dictate what it'll accept
      int method = ssh_userauth_list(this->session_, user.c_str());
      if (method & SSH_AUTH_METHOD_PUBLICKEY)
      {
  		   rc = ssh_userauth_publickey_auto(this->session_, user.c_str(), nullptr);
  		   // TO DO - handle full set of pubkey auth conditions
		   if (rc == SSH_AUTH_ERROR)
  		   {
     		   std::cerr << "Authentication failed: " <<	ssh_get_error(this->session_)
				   		 << std::endl;
  			   throw ssh::SshException(this->session_);
		   }
         else if (rc == SSH_AUTH_DENIED)
         {
     		   std::cerr << "Public key auth denied for user '"
                      << user
                      << "'; moving onto next method."
				   		 << std::endl;
         }
         else if (rc == SSH_AUTH_PARTIAL)
         {
     		   std::cerr << "Partial public key auth accepted for user '"
                      << user
                      << "'; moving onto next method."
				   		 << std::endl;
         }
         else if (rc == SSH_AUTH_SUCCESS)
         {
            std::cout << "*--Successfully authenticated user via public key." << std::endl;
		      return true;
         }
      }

      if (method & SSH_AUTH_METHOD_PASSWORD)
      {
  		   std::unique_ptr<char> password(getpass("Enter your password: "));
  		   // TO DO - get rid of password(), use custom password echo function
		   rc = ssh_userauth_password(this->session_, user.c_str(), password.get());
  		   if (rc == SSH_AUTH_ERROR)
  		   {
     		   std::cerr << "Authentication failed: "
				   		 << ssh_get_error(this->session_);
  			   throw ssh::SshException(this->session_);
  		   }
         else if (rc == SSH_AUTH_DENIED)
         {
     		   std::cerr << "Password auth denied for user '"
                      << user
                      << "'."
				   		 << std::endl;
         }
         else if (rc == SSH_AUTH_PARTIAL)
         {
     		   std::cerr << "Partial password auth accepted for user '"
                      << user
                      << "'; moving onto next method."
				   		 << std::endl;
         }
         else if (rc == SSH_AUTH_SUCCESS)
         {
            std::cout << "*--Successfully authenticated user via password." << std::endl;
		      return true;
         }
      }

      if (method & SSH_AUTH_METHOD_INTERACTIVE)
      {
		   // TO DO
      }

   }

   std::cerr << "Could not negotiate acceptable auth for user '"
                      << user
                      << "' with remote host."
				   		 << std::endl;
   return false;
}

void sftp_server::set_log_verbosity(sftp_log_level level)
{

}

}
