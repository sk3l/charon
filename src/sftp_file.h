#ifndef SFTP_FILE_H
#define SFTP_FILE_H

#include <sys/stat.h>

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
         uint8_t     get_type() const;
         uint64_t    get_size() const;
         std::string get_size_str() const;
         uint32_t    get_uid() const;
         uint32_t    get_gid() const;
         std::string get_owner() const;
         std::string get_group() const;
         uint32_t    get_permissions() const;
         date_time   get_access_time() const;
         date_time   get_create_time() const;
         date_time   get_mod_time() const;

         bool        is_directory() const;
         bool        is_file() const;

         ~sftp_file();
   };


   template<typename charT=char, typename traits=std::char_traits<charT> >
   std::basic_ostream<charT,traits> & operator<<
   (
      std::basic_ostream<charT,traits> & os, 
      sftp_file & sd
   )
   {
      uint32_t mode = sd.get_permissions();
      std::basic_string<charT,traits> perms;

      if (mode & S_IFDIR) perms += "d"; else perms += "-";
      if (mode & S_IRUSR) perms += "r"; else perms += "-";
      if (mode & S_IWUSR) perms += "w"; else perms += "-";
      if (mode & S_IXUSR) perms += "x"; else perms += "-";
      if (mode & S_IRGRP) perms += "r"; else perms += "-";
      if (mode & S_IWGRP) perms += "w"; else perms += "-";
      if (mode & S_IXGRP) perms += "x"; else perms += "-";
      if (mode & S_IROTH) perms += "r"; else perms += "-";
      if (mode & S_IWOTH) perms += "w"; else perms += "-";
      if (mode & S_IXOTH) perms += "x"; else perms += "-";

      os << std::setw(11) << perms
         << std::setw(9)  << sd.get_owner()
         << std::setw(9)  << sd.get_group()
         << std::setw(6)  << sd.get_size_str()
         << std::setw(9)  << sd.get_mod_time().format_str("%Y%m%d") 
         << std::setw(8)  << sd.get_mod_time().format_str("%H:%S") 
         << sd.get_name() << std::endl;

      return os;
   }
}

#endif // SFTP_FILE_H
