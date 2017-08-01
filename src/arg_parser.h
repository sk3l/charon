#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <memory>
#include <string>

#include "app/parameters/app_params.h"
#include "app/parameters/app_signature.h"

namespace charon {

   class arg_parser 
   {
      private :
         sk3l::app::app_signature app_sig_;

      public :

         using arg = std::shared_ptr<sk3l::app::app_param>;
         
         arg_parser();

         arg operator[](const std::string & name);

         bool validate(int argc, char ** argv);
         void print_usage() const;

         
   };
}

#endif // ARG_PARSER_H
