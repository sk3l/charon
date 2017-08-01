#include <exception>
#include <iostream>
#include <sstream>

#include "arg_parser.h"

namespace charon {

arg_parser::arg_parser()
: app_sig_("charon", "A bare-bones SFTP client")
{
   this->app_sig_.add_positionial
   (
      "user@host[:port]", 
      "remote host to connect to (default port=22)"
   );
}

arg_parser::arg arg_parser::operator[](const std::string & name)
{
   return this->app_sig_[name];
}

bool arg_parser::parse(int argc, char ** argv)
{
   if (this->app_sig_.parse(argc, argv, false) != sk3l::app::app_signature::OK)
   {
      this->app_sig_.print_usage();
      return false;
   }

   return true;
}

void arg_parser::print_usage() const
{
   this->app_sig_.print_usage();
}

}
