#include <iostream>
#include <sstream>
#include <string>

//#include "../app/logging/log_util.h"
#include <libssh/libsshpp.hpp>
#include "app/parameters/app_signature.h"
//#include "../core/datetime/date_time.h"

#include "sftp_server.h"

//#include "data/adt/linked_list.h"
//#include "data/adt/red_black_tree.h"
//#include "data/adt/stack.h"

/*
void test_date_time()
{
   using sk3l_date_time = sk3l::core::datetime::date_time;

   std::cout   << std::endl
               << "***** Date Time Tests *****" << std::endl;
   std::time_t tmt =  std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

   char now[100];
   std::strftime(now, sizeof(now)-1, "%FT%T", std::gmtime(&tmt));

   std::cout << std::endl << "Current time (tm) is " << now << std::endl;

   sk3l_date_time dt(sk3l_date_time::now());
   std::cout << "Now total ms => " << dt.get_total_milliseconds() << std::endl;
   std::cout << "Now total sec=> " << dt.get_total_seconds() << std::endl;
   std::cout << "Now total min=> " << dt.get_total_minutes() << std::endl;
   std::cout << "Now total hr => " << dt.get_total_hours() << std::endl;
   std::cout << "Now total day=> " << dt.get_total_days() << std::endl;
   std::cout << "Now total yr => " << dt.get_total_years() << std::endl;


   short dateParts[7];
   dt.get_date_time_parts(
      dateParts[0],
      dateParts[1],
      dateParts[2],
      dateParts[3],
      dateParts[4],
      dateParts[5],
      dateParts[6]);

   std::cout << std::endl;
   std::cout << "Date parts (1 call): " << dateParts[0] << "-" << dateParts[1] << "-" << dateParts[2]
             << "T" << dateParts[3] << ":" << dateParts[4] << ":" << dateParts[5] << "."
             << dateParts[6] << std::endl;

   std::cout << "Date parts (mult. calls): " << dt.get_year() << "-" << dt.get_month() << "-"
             << dt.get_day() << "T" << dt.get_hours() << ":" << dt.get_minutes() << ":"
             << dt.get_seconds() << "." << dt.get_milliseconds() << std::endl;
   std::cout << std::endl;
   std::cout <<  "ISO date format: " << dt.format_iso_date_str() << std::endl;
   std::cout <<  "ISO time format: " << dt.format_iso_time_str() << std::endl;

   struct tm t = {};
   t.tm_year   = 76;
   t.tm_mon    = 8;
   t.tm_mday   = 29;
   t.tm_hour   = 0;
   t.tm_min    = 0;
   t.tm_isdst  = 0;
   t.tm_zone   = "UTC";

   std::time_t bd = mktime(&t);

   sk3l_date_time bdt(sk3l::core::datetime::long_clock::from_time_t(bd));

   std::cout << std::endl << "Birthday is " << bdt << std::endl;

   sk3l_date_time phiaBD;
   phiaBD.set_date_time_parts
   (
      2009,
      sk3l::core::datetime::calendar::months::AUGUST,
      28,
      8,
      30,
      28,
      200
   );
   std::cout << "Sophia birthday is " << phiaBD << std::endl;
   std::cout << "Sophia birthday ticks are " << phiaBD.get_total_milliseconds() << std::endl;

   sk3l_date_time nxtDate = phiaBD + std::chrono::hours(24);
   std::cout << "Sophia birthday + 1 day is " << nxtDate << std::endl;
   std::cout << std::endl
             <<  "***** End Date Time Tests *****" << std::endl
             << std::endl;
}
*/
int main(int argc, char ** argv)
{
/*
   using bool_param_t = sk3l::app::switch_app_param;

   sk3l::app::app_signature appsig;

   std::shared_ptr<bool_param_t> ap1(new bool_param_t("conf", "", "", -1, false));
   appsig.insert(ap1);

   std::shared_ptr<bool_param_t> ap2(new bool_param_t("log", "", "", -1, false));
   appsig.insert(ap2);

   std::shared_ptr<bool_param_t> ap3(new bool_param_t("stack", "", "", -1, false));
   appsig.insert(ap3);

   std::shared_ptr<bool_param_t> ap4(new bool_param_t("rbtree", "", "", -1, false));
   appsig.insert(ap4);

   std::shared_ptr<bool_param_t> ap5(new bool_param_t("list", "", "", -1, false));
   appsig.insert(ap5);

   std::shared_ptr<bool_param_t> ap6(new bool_param_t("datetime", "", "", -1, false));
   appsig.insert(ap6);

   std::shared_ptr<bool_param_t> ap7(new bool_param_t("tcpc", "", "", -1, false));
   appsig.insert(ap7);

   std::shared_ptr<bool_param_t> ap8(new bool_param_t("tcps", "", "", -1, false));
   appsig.insert(ap8);

   std::shared_ptr<bool_param_t> ap9(new bool_param_t("tcpm", "", "", -1, false));
   appsig.insert(ap9);

   std::shared_ptr<bool_param_t> ap10(new bool_param_t("string", "", "", -1, false));
   appsig.insert(ap10);

   std::shared_ptr<bool_param_t> ap11(new bool_param_t("json", "", "", -1, false));
   appsig.insert(ap11);

   std::shared_ptr<bool_param_t> ap12(new bool_param_t("xml", "", "", -1, false));
   appsig.insert(ap12);

   std::shared_ptr<bool_param_t> ap13(new bool_param_t("xmlf", "", "", -1, false));
   appsig.insert(ap13);

   if (!appsig.validate(argc, argv))
   {
      std::cout << "Invalid args." << std::endl;
      return 16;
   }
*/
   try
   {
      try
      {
         charon::sftp_server server("localhost", 22);
         if (!server.connect(argv[1]))
         {
            std::cerr << "Unable to connect to remote SFTP host." << std::endl;
            exit(8);
         }
         std::cout << "*--Successfully connected to remote SFTP host." << std::endl;
      }
      catch (ssh::SshException & sshe)
      {
         std::cerr << "Error attaching to SFTP server : " << sshe.getError() << std::endl;
         exit(16);
      }

      std::cout << "charon: bare-bones SFTP client" << std::endl;
   }
   catch (const std::exception & e)
   {
      std::cout << "Error in charon::main: " << e.what() << std::endl;
   }

   return 0;
}

