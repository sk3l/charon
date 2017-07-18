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
}

sftp_session::~sftp_session()
{
}

}
