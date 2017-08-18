#include <iostream>
#include <memory>
#include <sstream>

#include <string.h>

#include <sftp_connection.h>
#include "sftp_server.h"


namespace charon {

sftp_server::sftp_server(const std::string & host, short port)
   : host_(host),
     port_(port)
{
}

sftp_conn_ptr sftp_server::connect(const std::string & user)
{
   return sftp_conn_ptr(new sftp_connection(user, this->host_, this->port_));
}

bool sftp_server::is_connected() const
{
   // TO DO : track created connections & report them
   return true;
}

}
