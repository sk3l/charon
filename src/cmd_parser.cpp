#include <exception>
#include <iostream>
#include <sstream>

#include "core/text/string_util.h"

#include "cmd_parser.h"

using string_util = sk3l::core::text::string_util;

namespace charon {

cmd_parser::cmd_parser()
   : cmd_map_(128, sk3l::data::adt::string_hash()) 
{
   cmd_map_.insert("quit", cmd_type::QUIT);
   cmd_map_.insert("help", cmd_type::HELP);
   cmd_map_.insert("list", cmd_type::LIST);
}

cmd_data cmd_parser::get_next_cmd()
{
   std::cout << "#> ";

   std::string line;
   if (!std::getline(std::cin, line))
   {
      std::cerr << "!! internal error; please try again.";
      return {cmd_type::ERROR, ""};
   }
   
   line = string_util::strip_ws(line);

   if (line.length() < 1)
   {
      std::cerr << "?? null command received; "
                << "please type 'help' for a list of commands, "
                << "or type 'quit' to exit."
                << std::endl;
      
      return {cmd_type::UNKNOWN, ""}; 
   }
  
   auto pos =line.find_first_of(" \t", 0);

   std::string cmd = line.substr(0, std::min(pos, line.length()));
  
   std::string args; 
   if (pos != std::string::npos)
      args = line.substr(++pos);

   auto cmd_it = this->cmd_map_.find(string_util::to_lower(cmd));
   if (cmd_it == this->cmd_map_.end())
   {
      std::cerr << "?? unknown command '" << cmd << "' received; "
                << "please type 'help' for a list of commands."
                << std::endl;

      return {cmd_type::UNKNOWN, ""}; 
   } 

   return {cmd_it->get_val(), args};
}

}
