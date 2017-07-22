#ifndef SFTP_DIR_H
#define SFTP_DIR_H

#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include <libssh/sftp.h>

namespace charon {

   using sftp_attrib_ptr = std::unique_ptr<sftp_attributes_struct>;

   class sftp_directory
   {
      private :
         sftp_session session_;
         std::string path_;
         std::vector<sftp_attrib_ptr> attributes_;

      public :

         sftp_directory(sftp_session session, const std::string & path);
         sftp_directory(const sftp_directory & rhs);
         sftp_directory operator=(const sftp_directory & rhs);


         ~sftp_directory();

         using sftp_attrib_iter = std::vector<sftp_attrib_ptr>::iterator;
         
         inline sftp_attrib_iter begin() {return this->attributes_.begin();}
         inline sftp_attrib_iter end()   {return this->attributes_.end();} 

   };


   template<typename charT=char, typename traits=std::char_traits<charT> >
   std::basic_ostream<charT,traits> & operator<<
   (
      std::basic_ostream<charT,traits> & os, 
      sftp_directory & sd
   )
   {
      /* 
      printf("%-20s %10llu %.8o %s(%d)\t%s(%d)\n",
      attributes->name,
      (long long unsigned int) attributes->size,
      attributes->permissions,
      attributes->owner,
      attributes->uid,
      attributes->group,
      attributes->gid);
      sftp_attributes_free(attributes);
      */
      os << "Name                       Size Perms    Owner\tGroup\n";
      for (auto it = sd.begin(); it != sd.end(); ++it)
      {
         os << it->get()->name            << " "
            << it->get()->size            << " "
            << it->get()->permissions     << " "
            << it->get()->owner           << " "
            << it->get()->uid             << " "
            << it->get()->group           << " "
            << it->get()->gid             << " "
            << std::endl;
      }
      

      return os;
   }
}

#endif // SFTP_DIR_H
