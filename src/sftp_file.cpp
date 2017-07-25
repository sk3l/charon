#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
#include <ctime>

#include <string.h>

#include "core/datetime/long_clock.h"

#include "sftp_file.h"

namespace charon {

sftp_file::sftp_file()
: file_(new struct sftp_attributes_struct())
{}

sftp_file::sftp_file(const sftp_attributes & attrs)
: file_(new struct sftp_attributes_struct())
{
   this->file_->name                = strdup(attrs->name);
   this->file_->longname            = strdup(attrs->longname);
   this->file_->size                = attrs->size;
   this->file_->uid                 = attrs->uid;
   this->file_->gid                 = attrs->gid;
   this->file_->owner               = strdup(attrs->owner);
   this->file_->group               = strdup(attrs->group);
   this->file_->permissions         = attrs->permissions;
   this->file_->atime64             = attrs->atime64;
   this->file_->atime               = attrs->atime;
   this->file_->atime_nseconds      = attrs->atime_nseconds;
   this->file_->createtime          = attrs->createtime;
   this->file_->createtime_nseconds = attrs->createtime_nseconds;
   this->file_->mtime64             = attrs->mtime64;
   this->file_->mtime               = attrs->mtime;
   this->file_->mtime_nseconds      = attrs->mtime_nseconds;

   this->file_->extended_count      = attrs->extended_count;
}

sftp_file::sftp_file(sftp_attributes && attrs)
: file_(attrs)
{
   attrs = nullptr;
}

sftp_file::sftp_file(const sftp_file & rhs)
: file_(new struct sftp_attributes_struct())
{
   this->file_->name                = strdup(rhs.file_->name);
   this->file_->longname            = strdup(rhs.file_->longname);

   this->file_->size                = rhs.file_->size;
   this->file_->uid                 = rhs.file_->uid;
   this->file_->gid                 = rhs.file_->gid;

   this->file_->owner               = strdup(rhs.file_->owner);
   this->file_->group               = strdup(rhs.file_->group);

   this->file_->permissions         = rhs.file_->permissions;
   this->file_->atime64             = rhs.file_->atime64;
   this->file_->atime               = rhs.file_->atime;
   this->file_->atime_nseconds      = rhs.file_->atime_nseconds;
   this->file_->createtime          = rhs.file_->createtime;
   this->file_->createtime_nseconds = rhs.file_->createtime_nseconds;
   this->file_->mtime64             = rhs.file_->mtime64;
   this->file_->mtime               = rhs.file_->mtime;
   this->file_->mtime_nseconds      = rhs.file_->mtime_nseconds;

   this->file_->extended_count      = rhs.file_->extended_count;
}

sftp_file & sftp_file::operator=(const sftp_file & rhs)
{
   if (this != &rhs)
   {
      free(this->file_->name);
      this->file_->name                = strdup(rhs.file_->name);
      free(this->file_->longname);
      this->file_->longname            = strdup(rhs.file_->longname);
   
      this->file_->size                = rhs.file_->size;
      this->file_->uid                 = rhs.file_->uid;
      this->file_->gid                 = rhs.file_->gid;
   
      free(this->file_->owner);
      this->file_->owner               = strdup(rhs.file_->owner);
      free(this->file_->group);
      this->file_->group               = strdup(rhs.file_->group);
   
      this->file_->permissions         = rhs.file_->permissions;
      this->file_->atime64             = rhs.file_->atime64;
      this->file_->atime               = rhs.file_->atime;
      this->file_->atime_nseconds      = rhs.file_->atime_nseconds;
      this->file_->createtime          = rhs.file_->createtime;
      this->file_->createtime_nseconds = rhs.file_->createtime_nseconds;
      this->file_->mtime64             = rhs.file_->mtime64;
      this->file_->mtime               = rhs.file_->mtime;
      this->file_->mtime_nseconds      = rhs.file_->mtime_nseconds;
   
      this->file_->extended_count      = rhs.file_->extended_count;
   }

   return *this;
}

std::string sftp_file::get_name() const
{
   return std::string(this->file_->name);
}

std::string sftp_file::get_long_name() const
{
   return std::string(this->file_->longname);
}

uint32_t    sftp_file::get_flags() const
{
   return this->file_->flags;
}

uint64_t    sftp_file::get_size() const
{
   return this->file_->size;
}

uint32_t    sftp_file::get_uid() const
{
   return this->file_->uid;
}

uint32_t    sftp_file::get_gid() const
{
   return this->file_->gid;
}

std::string sftp_file::get_owner() const
{
   return std::string(this->file_->owner);
}

std::string sftp_file::get_group() const
{
   return std::string(this->file_->group);
}

uint32_t    sftp_file::get_permissions() const
{
   return this->file_->permissions;
}

date_time   sftp_file::get_access_time() const
{
   return
      date_time(
         sk3l::core::datetime::long_clock::from_time_t
         (
            std::time_t(this->file_->atime) 
         )
      );
}

date_time   sftp_file::get_create_time() const
{
   return
      date_time(
         sk3l::core::datetime::long_clock::from_time_t
         (
            std::time_t(this->file_->createtime) 
         )
      );
}

date_time   sftp_file::get_mod_time() const
{
   return
      date_time(
         sk3l::core::datetime::long_clock::from_time_t
         (
            std::time_t(this->file_->mtime) 
         )
      );
}

sftp_file::~sftp_file()
{
   sftp_attributes_free(this->file_);
   this->file_ = nullptr;
}

}