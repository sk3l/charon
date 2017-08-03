#ifndef CMD_PARSER_H
#define CMD_PARSER_H

#include <memory>
#include <string>

#include "data/adt/hash_map_o.h"

namespace charon {

   enum cmd_type
   {
      ERROR    = -2,
      UNKNOWN  = -1,
      QUIT     = 0,
      HELP     = 1,
      LIST     = 2
   };
  
   struct cmd_data
   {
      cmd_type    type_;
      std::string parameters_;
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
