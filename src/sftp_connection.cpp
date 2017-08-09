#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
#include <string.h>

#include <libssh/sftp.h>

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

   return sftp_file(attrib);
}

}
