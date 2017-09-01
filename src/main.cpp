#include <iostream>
#include <sstream>
#include <string>

//#include "../app/logging/log_util.h"
#include <libssh/libsshpp.hpp>
#include "app/parameters/app_signature.h"
//#include "../core/datetime/date_time.h"

#include "arg_parser.h"
#include "cmd_parser.h"
#include "sftp_connection.h"
#include "sftp_directory.h"
#include "sftp_server.h"

using string_util = sk3l::core::text::string_util;

int main(int argc, char ** argv)
{
   try
   {
      charon::arg_parser ap;

      if (!ap.parse(argc, argv))
         exit(16);

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

         charon::cmd_parser cp;
         for
         (
            charon::cmd_data cmd_to_do = cp.get_next_cmd();
            cmd_to_do.type_ != charon::cmd_type::QUIT;
            cmd_to_do = cp.get_next_cmd()
         )
         {
            try
            {
               switch (cmd_to_do.type_)
               {
                  case charon::cmd_type::HELP:
                     std::cout << "Help is coming..." << std::endl;
                  break;

                  case charon::cmd_type::LIST:
                  {
                     std::string path;
                     if (cmd_to_do.parameters_.size() > 0)
                        path = string_util::strip_ws(cmd_to_do.parameters_[0]);
                     else
                        path = "./";         // default to current directory

                     charon::sftp_directory dir = conn->read_directory(path);
                     std::cout << dir;
                  }
                  break;

                  case charon::PWD:
                     conn->print_working_directory();
                  break;

                  case charon::CD:
                  {
                     std::string path;
                     if (cmd_to_do.parameters_.size() > 0)
                        path = string_util::strip_ws(cmd_to_do.parameters_[0]);
                     else
                        path = "./";         // default to current directory

                     conn->change_directory(path);
                     std::cout << "Changed directory to " << path << std::endl;
                  }
                  break;

                  case charon::STAT:
                  {
                     std::string path;
                     if (cmd_to_do.parameters_.size() > 0)
                        path = string_util::strip_ws(cmd_to_do.parameters_[0]);
                     else
                     {
                        std::cerr << "Must provide argument to stat (e.g. stat <foo>)"
                                  << std::endl;
                        continue;
                     }

                     charon::sftp_file f = conn->stat(path);
                     f.print_stat();
                  }
                  break;

                  case charon::cmd_type::PUT:
                  {
                     size_t argcnt = cmd_to_do.parameters_.size();

                     if (argcnt < 1 || argcnt > 2)
                     {
                        std::cerr << "Must provide argument to put (e.g. put <src> [<dest>])"
                                  << std::endl;
                        continue;
                     }

                     std::string src = string_util::strip_ws(cmd_to_do.parameters_[0]);
                     std::string dest = src;

                     if (argcnt == 2)
                        dest = string_util::strip_ws(cmd_to_do.parameters_[1]);

                     conn->put(src, dest);
                  }
                  break;

                  case charon::cmd_type::ERROR:
                  default:
                     std::cerr << "Unspecified error parsing SFTP command. "
                               << "Please try again."
                               << std::endl;
                  break;
               }
            }
            catch (const std::exception & err)
            {
               std::cerr << err.what() << std::endl;
            }
         }

         std::cout << "charon is bringing you home." << std::endl;
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
      exit(16);
   }

   return 0;
}

