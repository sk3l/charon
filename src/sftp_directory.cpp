#include <exception>
#include <iostream>
#include <memory>
#include <sstream>

#include <string.h>

#include "sftp_server.h"
#include "sftp_directory.h"


namespace charon {

sftp_directory::sftp_directory(sftp_session session, const std::string & path)
   : session_(session),
     path_(path)
{

   sftp_dir dir;
   sftp_attributes attributes;
   int rc;

   dir = sftp_opendir(this->session_, path.c_str());
   if (!dir)
   {
      std::string err = "Couldn't open directory at '";
      err +=  path + "'";
      throw std::logic_error(err);
   }
   
   while ((attributes = sftp_readdir(this->session_, dir)) != nullptr)
   {
      this->files_.push_back
      (
         sftp_file_ptr(new sftp_file(std::move(attributes)))
      );
   }

   if (!sftp_dir_eof(dir))
   {
      std::string err = "Error reading directory at '";
      err +=  path + "': ";
      err += ssh_get_error(session);
      throw std::logic_error(err);
      sftp_closedir(dir);
   }
   
   rc = sftp_closedir(dir);
   if (rc != SSH_OK)
   {
      std::string err = "Error closing directory at '";
      err +=  path + "' after read: ";
      err += ssh_get_error(session);
      throw std::logic_error(err);
   }

}

sftp_directory::sftp_directory(const sftp_directory & rhs)
   : session_(rhs.session_),
     path_(rhs.path_),
     files_(rhs.files_)
{
}

sftp_directory::~sftp_directory()
{
}

}
