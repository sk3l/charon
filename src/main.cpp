#include <iostream>
#include <sstream>
#include <string>

//#include "../app/logging/log_util.h"
#include <libssh/libsshpp.hpp>
#include "app/parameters/app_signature.h"
//#include "../core/datetime/date_time.h"

#include "arg_parser.h"
#include "sftp_connection.h"
#include "sftp_directory.h"
#include "sftp_server.h"

using string_util = sk3l::core::text::string_util; 

int main(int argc, char ** argv)
{
   try
   {
      charon::arg_parser ap;

      if (!ap.validate(argc, argv))
      {
         ap.print_usage();
         exit(16);
      }

      try
      {
         std::string endpoint =
            ap["user@host[:port]"]->get_value();

         auto at = endpoint.find("@");
         auto colon = endpoint.find(":");

         std::string user = endpoint.substr(0, at++);

         std::string host;
         int port = 22;

         if (colon != std::string::npos)
         {
            host = endpoint.substr(at, endpoint.size() - at);
            port = string_util::string_to_numeric<int>(endpoint.substr(++colon)); 
         }
         else
         {
            host = endpoint.substr(at);
         }

         charon::sftp_server server(host, port);

         charon::sftp_conn_ptr conn = server.connect(user);
         if (!conn)
         {
            std::cerr << "Unable to connect to remote SFTP host." << std::endl;
            exit(8);
         }
         std::cout << "*--Successfully connected to remote SFTP host." << std::endl;

         charon::sftp_directory dir = conn->read_directory("Code");

         std::cout << dir; 
      }
      catch (ssh::SshException & sshe)
      {
         std::cerr << "Error attaching to SFTP server : " << sshe.getError() << std::endl;
         exit(16);
      }

   }
   catch (const std::exception & e)
   {
      std::cout << "Error in charon::main: " << e.what() << std::endl;
   }

   return 0;
}

