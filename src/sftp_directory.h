#ifndef SFTP_DIR_H
#define SFTP_DIR_H

#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include <libssh/sftp.h>

#include "sftp_file.h"

namespace charon {

   using sftp_file_ptr = std::shared_ptr<sftp_file>;

   class sftp_directory
   {
      private :
         sftp_session session_;
         std::string path_;
         std::vector<sftp_file_ptr> files_;

      public :

         sftp_directory(sftp_session session, const std::string & path);
         sftp_directory(const sftp_directory & rhs);
         sftp_directory operator=(const sftp_directory & rhs);

         ~sftp_directory();

         using sftp_file_iter = std::vector<sftp_file_ptr>::iterator;
         
         inline sftp_file_iter begin() {return this->files_.begin();}
         inline sftp_file_iter end()   {return this->files_.end();} 

   };


   template<typename charT=char, typename traits=std::char_traits<charT> >
   std::basic_ostream<charT,traits> & operator<<
   (
      std::basic_ostream<charT,traits> & os, 
      sftp_directory & sd
   )
   {
      os << "Name                       Size Perms    Owner\tGroup\n";
      for (auto it = sd.begin(); it != sd.end(); ++it)
         os << *it->get();

      return os;
   }
}

#endif // SFTP_DIR_H
