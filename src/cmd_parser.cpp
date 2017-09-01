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
   cmd_map_.insert("q",    cmd_type::QUIT);
   cmd_map_.insert("help", cmd_type::HELP);
   cmd_map_.insert("h",    cmd_type::HELP);
   cmd_map_.insert("list", cmd_type::LIST);
   cmd_map_.insert("ls",   cmd_type::LIST);
   cmd_map_.insert("pwd",  cmd_type::PWD);
   cmd_map_.insert("cd",   cmd_type::CD);
   cmd_map_.insert("stat", cmd_type::STAT);
   cmd_map_.insert("put",  cmd_type::PUT);
}

cmd_data cmd_parser::get_next_cmd()
{
   std::cout << "#> ";

   std::string line;
   if (!std::getline(std::cin, line))
   {
      std::cerr << "!! internal error; please try again.";
      return {cmd_type::ERROR, {}};
   }

   line = string_util::strip_ws(line);

   if (line.length() < 1)
   {
      std::cerr << "?? null command received; "
                << "please type 'help' for a list of commands, "
                << "or type 'quit' to exit."
                << std::endl;

      return {cmd_type::UNKNOWN, {}};
   }

   auto pos = line.find_first_of(" \t");
   size_t len = line.length();

   std::string cmd = line.substr(0, (pos == std::string::npos) ? len : pos);

   cmd_param_list args;
   while ((pos != std::string::npos) && (++pos < len))
   {
      auto nxtpos = line.find_first_of(" \t", pos);
      size_t l = ((nxtpos == std::string::npos) ? len : nxtpos) - pos;

      if (l > 0)
         args.push_back(line.substr(pos, l));
      pos = nxtpos;
   }

   auto cmd_it = this->cmd_map_.find(string_util::to_lower(cmd));
   if (cmd_it == this->cmd_map_.end())
   {
      std::cerr << "?? unknown command '" << cmd << "' received; "
                << "please type 'help' for a list of commands."
                << std::endl;

      return {cmd_type::UNKNOWN, {}};
   }

   return {cmd_it->get_val(), args};
}

}
