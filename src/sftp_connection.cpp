#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
#include <string.h>

#include "sftp_connection.h"

namespace charon {

sftp_connection::sftp_connection(::ssh_session sess)
   : ssh_sess_(sess)
{
   if (!sess)
      throw std::invalid_argument("Invalid SSH session passed to sftp_connection().");

   std::unique_ptr<sftp_session_struct> tmp(sftp_new(sess));
   if (tmp == nullptr)
      throw std::bad_alloc();

   int rc = sftp_init(tmp.get());
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
}

std::string sftp_connection::canonicalize(const std::string & path)
{
   return sftp_canonicalize_path(this->sftp_sess_, path.c_str());
}

void sftp_connection::change_directory(const std::string & dir)
{
   // TO DO : need path validation logic
   this->cwd_ = dir;
}

void sftp_connection::print_working_directory() const
{
   std::cout << "Current working director : " << this->cwd_ << std::endl;
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

}
