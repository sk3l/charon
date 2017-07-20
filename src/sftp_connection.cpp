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
      this->sftp_sess_ = tmp.release();
   else
      throw std::runtime_error("Failed to initialize sftp_connection.");
}

sftp_connection::~sftp_connection()
{
   sftp_free(this->sftp_sess_);
   this->sftp_sess_ = nullptr;
}

}
