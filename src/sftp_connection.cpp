#include <exception>
#include <iostream>
#include <memory>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <string.h>

#include <libssh/sftp.h>
#include <libssh/libsshpp.hpp>

#include "sftp_connection.h"

namespace charon {

bool sftp_connection::authenticate_server()
{
   int rc, state;
   size_t hlen;
   unsigned char *hash = NULL;
   char buf[10];

   bool auth_ok = true;

   ssh_key server_key;

   state = ssh_is_server_known(this->ssh_sess_);

   rc = ssh_get_publickey(this->ssh_sess_, &server_key);
   if (rc != SSH_OK)
      throw ssh::SshException(this->ssh_sess_);

   rc = ssh_get_publickey_hash(server_key, SSH_PUBLICKEY_HASH_MD5, &hash, &hlen);
   if (rc != SSH_OK)
      throw ssh::SshException(this->ssh_sess_);

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

         if (ssh_write_knownhost(this->ssh_sess_) < 0)
         {
            std::cerr << "Error " << strerror(errno) << std::endl;
            auth_ok = false;
            break;
         }
      }

      case SSH_SERVER_ERROR:
         std:: cerr << "Error " << ssh_get_error(this->ssh_sess_) << std::endl;
         throw ssh::SshException(this->ssh_sess_);

   }

   free(hash);
   return auth_ok;

}

bool sftp_connection::authenticate_user(const std::string & user)
{
   int rc;

   // First try anonymous access (auth = 'none')
   rc = ssh_userauth_none(this->ssh_sess_, user.c_str());
   if (rc == SSH_AUTH_ERROR)
   {
      std::cerr << "Authentication failed: " <<   ssh_get_error(this->ssh_sess_)
                 << std::endl;
      throw ssh::SshException(this->ssh_sess_);
   }
   else if (rc == SSH_AUTH_SUCCESS)
   {
      return true;
   }
   else
   {
      // Now let the server dictate what it'll accept
      int method = ssh_userauth_list(this->ssh_sess_, user.c_str());
      if (method & SSH_AUTH_METHOD_PUBLICKEY)
      {
         rc = ssh_userauth_publickey_auto(this->ssh_sess_, user.c_str(), nullptr);
         // TO DO - handle full set of pubkey auth conditions
         if (rc == SSH_AUTH_ERROR)
         {
            std::cerr << "Authentication failed: " <<   ssh_get_error(this->ssh_sess_)
                      << std::endl;
            throw ssh::SshException(this->ssh_sess_);
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
         rc = ssh_userauth_password(this->ssh_sess_, user.c_str(), password.get());
         if (rc == SSH_AUTH_ERROR)
         {
            std::cerr << "Authentication failed: "
                      << ssh_get_error(this->ssh_sess_);
            throw ssh::SshException(this->ssh_sess_);
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


sftp_connection::sftp_connection(const std::string & user, const std::string & host, short port)
   : ssh_sess_(ssh_new())
{
   if (ssh_options_set(this->ssh_sess_, SSH_OPTIONS_HOST, host.c_str()) != SSH_OK)
   {
      ssh_free(this->ssh_sess_);
      throw std::logic_error("Encountered error assigning sftp_server host.");
   }

   long lport = port;
   if (ssh_options_set(this->ssh_sess_, SSH_OPTIONS_PORT,  &lport) != SSH_OK)
   {
      ssh_free(this->ssh_sess_);
      throw std::logic_error("Encountered error assigning sftp_server port.");
   }

   // Build the connection
   int rc = ssh_connect(this->ssh_sess_);
   if (rc != SSH_OK)
   {
      std::cerr << "Error connecting to " << host
                << ssh_get_error(this->ssh_sess_);

      throw ssh::SshException(this->ssh_sess_);
   }

   sftp_conn_ptr conn;
   // Authenticate the server
   if (!this->authenticate_server())
      throw std::logic_error("Unable to authenticate server.");

   // Authenticate the user
   if (!this->authenticate_user(user))
      throw std::logic_error("Unable to authenticate user.");

   std::unique_ptr<sftp_session_struct> tmp(sftp_new(this->ssh_sess_));
   if (tmp == nullptr)
      throw std::bad_alloc();

   rc = sftp_init(tmp.get());
   if (rc == SSH_OK)
   {
      char * workingDir = sftp_canonicalize_path(tmp.get(), "./");
      if (workingDir == nullptr)
         throw std::runtime_error("Failed to initialize SFTP working directory.");
      this->cwd_ = workingDir;

      this->sftp_sess_ = tmp.release();
   }
   else
      throw std::runtime_error("Failed to initialize sftp_connection.");
}

sftp_connection::~sftp_connection()
{
   sftp_free(this->sftp_sess_);
   this->sftp_sess_ = nullptr;

   ssh_free(this->ssh_sess_);
   this->ssh_sess_ = nullptr;
}

std::string sftp_connection::canonicalize(const std::string & path)
{
   char * canonicalPath = sftp_canonicalize_path(this->sftp_sess_, path.c_str());
   if (!canonicalPath)
   {
      std::string err = "Couldn't determine absolute path for '";
      err += path + "'";
      throw std::logic_error(err);
   }

   return std::string(canonicalPath);
}

void sftp_connection::change_directory(const std::string & dir)
{
   if (dir.length() < 1)
      return;

   std::string newPath;
   if (dir[0] == '/')
      newPath = dir;
   else
      newPath = this->cwd_ + "/" + dir;

   std::string canonicalPath = this->canonicalize(newPath);
   sftp_file obj = this->stat(canonicalPath);
   if (!obj.is_directory())
   {
      std::string err = "Couldn't change to non-directory path '";
      sftp_conn_ptr conn;
      err +=  canonicalPath + "'";
      throw std::logic_error(err);
   }

   this->cwd_ = canonicalPath;
}

void sftp_connection::print_working_directory() const
{
   std::cout << "Current working directory is " << this->cwd_ << std::endl;
}

sftp_directory sftp_connection::read_directory(const std::string & path)
{
   std::string realPath;
   // TO DO : need path validation logic
   if ((path.length() > 0) && (path[0] != '/'))
      realPath = this->cwd_ + "/" + path;
   else
      realPath = path;

   return sftp_directory(this->sftp_sess_, realPath);
}

sftp_file sftp_connection::stat(const std::string & path)
{
   sftp_attributes attrib;

   attrib = sftp_stat(this->sftp_sess_, path.c_str());
   if (!attrib)
   {
      std::string err = "Couldn't stat object at '";
      err +=  path + "'";
      throw std::logic_error(err);
   }

   if ((attrib->name == nullptr) || (strlen(attrib->name) < 1))
      attrib->name = const_cast<char*>(path.c_str());

   return sftp_file(attrib);
}

void sftp_connection::put(const std::string & lpath, const std::string & rpath)
{

   std::fstream local_file(lpath);
   if (!local_file)
      throw std::logic_error("Encountered error in put(): couldn't open file at local path '" + lpath + "'");

   ::sftp_file remote_file = 
      sftp_open
      (
         this->sftp_sess_, 
         rpath.c_str(), 
         O_WRONLY | O_CREAT | O_TRUNC,   // TO DO : make configurable
         S_IRWXU              // TO DO : make configurable 
      ); 

   if (remote_file == nullptr)
      throw std::logic_error("Encountered error in put(): couldn't open file at remote path '" + rpath + "'");

   // TO DO : configurable read buffer size
   const int BUFF_SIZE = 4 * (1 << 20);
   std::unique_ptr<char> buffer(new char[BUFF_SIZE]);
   try
   {
      int write_cnt = 0;
      do 
      {
         std::streamsize read_cnt = local_file.readsome(buffer.get(), BUFF_SIZE);
         if (!local_file || read_cnt < 1)
            break;

         write_cnt = sftp_write(remote_file, buffer.get(), read_cnt);
         if (write_cnt != read_cnt)
            throw std::logic_error("Encountered error int put(): I/O error writing remote file '" + rpath + "'");
      }
      while (local_file.good());
   
      if (!local_file.good() && !local_file.eof())
         throw std::logic_error("Encountered error int put(): I/O error reading local file '" + lpath + "'");

      sftp_close(remote_file);
   }
   catch (...)
   {
      sftp_close(remote_file);
      throw;
   }

}

void sftp_connection::get(const std::string & rpath, const std::string & lpath)
{

}

}
