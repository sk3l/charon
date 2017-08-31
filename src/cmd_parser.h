#ifndef CMD_PARSER_H
#define CMD_PARSER_H

#include <memory>
#include <string>
#include <vector>

#include "data/adt/hash_map_o.h"

namespace charon {

   enum cmd_type
   {
      ERROR    = -2,
      UNKNOWN  = -1,
      QUIT     = 0,
      HELP     = 1,
      LIST     = 2,
      PWD      = 3,
      CD       = 4,
      STAT     = 5,
      PUT      = 6
   };

   using cmd_param_list = std::vector<std::string>;

   struct cmd_data
   {
      cmd_type       type_;
      cmd_param_list parameters_;
   };

   class cmd_parser 
   {
      private :
         using cmd_map_t = sk3l::data::adt::open_hash_map<std::string,cmd_type>;

         cmd_map_t cmd_map_;

      public :

         cmd_parser();
         cmd_data get_next_cmd();         
   };
}

#endif // CMD_PARSER_H
