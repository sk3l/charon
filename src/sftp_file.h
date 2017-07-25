#ifndef SFTP_FILE_H
#define SFTP_FILE_H

#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include <libssh/sftp.h>

#include "core/datetime/date_time.h"

using sk3l::core::datetime::date_time;

namespace charon {

   using sftp_attrib_ptr = std::unique_ptr<sftp_attributes_struct>;

   class sftp_file
   {
      private :
         sftp_attributes file_;

      public :

         sftp_file();
         sftp_file(const sftp_attributes & attrs);
         sftp_file(sftp_attributes && attrs);
         sftp_file(const sftp_file & rhs);
         sftp_file & operator=(const sftp_file & rhs);

         std::string get_name() const;
         std::string get_long_name() const;
         uint32_t    get_flags() const;
         uint64_t    get_size() const;
         uint32_t    get_uid() const;
         uint32_t    get_gid() const;
         std::string get_owner() const;
         std::string get_group() const;
         uint32_t    get_permissions() const;
         date_time   get_access_time() const;
         date_time   get_create_time() const;
         date_time   get_mod_time() const;

         ~sftp_file();
   };


   template<typename charT=char, typename traits=std::char_traits<charT> >
   std::basic_ostream<charT,traits> & operator<<
   (
      std::basic_ostream<charT,traits> & os, 
      sftp_file & sd
   )
   {
         os << sd.get_name()        << " "
            << sd.get_size()        << " "
            << sd.get_permissions() << " "
            << sd.get_owner()       << " "
            << sd.get_uid()         << " "
            << sd.get_group()       << " "
            << sd.get_gid()         << " "
            << std::endl;

      return os;
   }
}

#endif // SFTP_FILE_H
