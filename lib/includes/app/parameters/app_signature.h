#ifndef APP_SIGNATURE_H
#define APP_SIGNATURE_H

#include <iostream>
#include <string>
#include <memory>
#include <typeinfo>
#include <vector>

#include "app_params.h"
#include "core/text/string_util.h"
#include "data/adt/hash_map_o.h"

namespace sk3l {
namespace app {


   // Encapsulates a collection of application pararmeters.
   template<typename charT = char,
            typename traits= std::char_traits<charT> >
   class basic_app_signature
   {
      private:
         using string_t       = std::basic_string<charT,traits>;

         using param_t        = basic_app_param<charT,traits>;
         using param_ptr_t    = std::shared_ptr<param_t>;
         using param_map_t    = sk3l::data::adt::open_hash_map<string_t, param_ptr_t>;

         using option_prm_t   = basic_optional_param<charT,traits>;
         using option_ptr_t   = std::shared_ptr<option_prm_t>;
         using option_map_t   = sk3l::data::adt::open_hash_map<string_t, option_ptr_t>;

         using position_prm_t = basic_positional_param<charT,traits>;
         using position_ptr_t = std::shared_ptr<position_prm_t>;
         using position_lst_t = std::vector<position_ptr_t>;

         static constexpr charT const * NAME_DELIM = "--";
         static constexpr charT const * ALIAS_DELIM = "-";


         string_t       app_name_;
         string_t       app_desc_;

         param_map_t    params_;
         option_map_t   optional_by_lname_;
         option_map_t   optional_by_sname_;
         position_lst_t positional_;

      public:
         basic_app_signature(const string_t & name, const string_t & desc = "")
            : app_name_(name),
              app_desc_(desc),
              params_(32, sk3l::data::adt::string_hash()),
              optional_by_lname_(32, sk3l::data::adt::string_hash()),
              optional_by_sname_(32, sk3l::data::adt::string_hash()),
              positional_()
         {}

         option_ptr_t add_optional
         (
            const string_t & name,
            const string_t & desc,
            const string_t & lname,
            const string_t & sname,
            bool is_switch = false
         )
         {
            option_ptr_t opt(new option_prm_t(name, desc, lname, sname, is_switch));
            this->params_.insert(name, opt);
            this->optional_by_lname_.insert(lname, opt);
            this->optional_by_sname_.insert(sname = opt);
            return opt;
         }

         position_ptr_t add_positionial
         (
            const string_t & name,
            const string_t & desc
         )
         {
            position_ptr_t pos(new position_prm_t(name, desc, this->positional_.size()+1));
            this->params_.insert(name, pos);
            this->positional_.push_back(pos);
            return pos;
         }

         param_ptr_t get_parameter(const string_t & name)
         {
            param_ptr_t p;

            auto p_it = this->params_.find(name);
            if (p_it != this->params_.end())
               p = p_it->get_val();

            return p;
         }

         param_ptr_t operator[](const string_t & name)
         {
            return this->get_parameter(name);
         }

         bool validate(int argc, charT ** args)
         {
            option_ptr_t option; // Keep track of previous option string

            auto positional_it = this->positional_.begin();

            for (int i = 1; i < argc; ++i) // Skip first argument, program name.
            {
               string_t arg(args[i]);

               if (!option)
               {
                  // Attempt to locate a parameter by long name
                  if(arg.substr(0,2) == string_t(NAME_DELIM))
                  {
                     size_t start = arg.find_first_not_of(NAME_DELIM);
                     string_t name =
                        sk3l::core::text::string_util::to_lower
                        (
                           arg.substr(start)
                        );

                     auto p = this->optional_by_lname_.find(name);
                     if (p == this->optional_by_lname_.end())
                        return false;

                     option = p->get_val();
                  }
                  // Attempt to locate a parameter by short name
                  else if(arg.substr(0,1) == string_t(ALIAS_DELIM))
                  {

                     size_t start = arg.find_first_not_of(ALIAS_DELIM);
                     string_t alias =
                        sk3l::core::text::string_util::to_lower
                        (
                           arg.substr(start)
                        );

                     auto p =  this->optional_by_sname_.find(alias);
                     if (p == this->optional_by_sname_.end())
                        return false;

                     option = p->get_val();
                  }
                  // Not an optional parameter; check against non-option list.
                  else if(positional_it == this->positional_.end())
                  {
                     // Unknown param
                     return false;
                  }

                  positional_it->get()->set_value(arg);
                  ++positional_it;
               }
               else if (option->get_is_switch())
               {
                  option->set_value("1");
                  option.reset();
               }
               else
               {
                  if
                  (
                     arg.substr(0,2) == string_t(NAME_DELIM) ||
                     arg.substr(0, 1) == string_t(ALIAS_DELIM)
                  )
                  {
                     // Consecutive optional params w/o a value
                     return false;
                  }

                  // Set the value for the previously encountered parameter.
                  option->set_value(arg);
                  option.reset();
               }
            }

            // Verify no trailing option w/o a value
            if (option)
               return false;

            // Verify all required params were present
            if (positional_it != this->positional_.end())
               return false;

            return true;
         }

         void print_usage() const
         {
            std::cout << this->app_name_;
            if (this->app_desc_.length() > 0)
               std::cout << " - " << this->app_desc_;
            std::cout << std::endl << std::endl;

            std::cout << "USAGE:" << std::endl << std::endl;

            std::cout << "    " << this->app_name_;

            std::stringstream ss;
            for
            (
               auto it = this->optional_by_sname_.begin();
               it != this->optional_by_sname_.end();
               ++it
            )
            {
               std::cout << " " << it->get_val()->get_usage_str();

               ss << it->get_val()->get_name();
               ss << " - "  << it->get_val()->get_desc();
               ss << std::endl;
            }

            for
            (
               auto it = this->positional_.begin();
               it != this->positional_.end();
               ++it
            )
            {
               std::cout << " " << it->get()->get_usage_str();

               ss << it->get()->get_name();
               ss << " - "  << it->get()->get_desc();
               ss << std::endl;
            }

            std::cout << std::endl << std::endl;
            std::cout << ss.str()  << std::endl;
         }

   };

   using app_signature  = basic_app_signature<char, std::char_traits<char> >;
   using wapp_signature = basic_app_signature<wchar_t, std::char_traits<wchar_t> >;

}
}

#endif
