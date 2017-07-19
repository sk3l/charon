#include <exception>
#include <iostream>
#include <memory>
#include <sstream>

#include <string.h>

#include <libssh/libsshpp.hpp>

#include "sftp_server.h"
#include "sftp_session.h"


namespace charon {

sftp_session::sftp_session(sftp_server * server)
   : server_(server)
{
   if (!server)
      throw std::invalid_argument("Invalid SFTP server passed to sftp_session().");

   std::unique_ptr<sftp_session_struct> tmp(sftp_new(server->get_ssh_session()));
   if (tmp == nullptr)
      throw std::bad_alloc();

   int rc = sftp_init(tmp.get());
   if (rc == SSH_OK)
      this->session_ = tmp.release();
   else
      throw std::runtime_error("Failed to initialize sftp_session.");
}

sftp_session::~sftp_session()
{
   sftp_free(this->session_);
   this->session_ = nullptr;
}

}
