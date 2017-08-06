#ifndef DATE_TIME_H
#define DATE_TIME_H

#include <chrono>
#include <cstdint>
#include <ctime>
#include <exception>
#include <iomanip>
#include <ostream>
#include <sstream>

#include "calendar.h"
#include "long_clock.h"

#include "core/text/string_util.h"

namespace sk3l{

namespace core{

namespace datetime{

template<typename charT>
inline charT get_fill_char()
{
   return charT(0);
}

template<>
inline char get_fill_char<char>()
{
   return '0';
}

template<>
inline wchar_t get_fill_char<wchar_t>()
{
   return L'0';
}


class date_time
{
public:

   using timepoint_t = sk3l::core::datetime::long_clock::time_point;
   using clock_t     = sk3l::core::datetime::long_clock;

   using count_t     = std::int64_t;
   using parts_t     = short;

protected:
   timepoint_t timestamp_;

   static const count_t LYEAR_DAYS     = 366;
   static const count_t YEAR_DAYS      = 365;
   static const count_t DAY_HOURS      = 24;
   static const count_t HOUR_MINUTES   = 60;
   static const count_t MINUTE_SECONDS = 60;

   static const count_t TICKS_PER_SECOND  = 1000;
   static const count_t TICKS_PER_MINUTE  = MINUTE_SECONDS  * TICKS_PER_SECOND;
   static const count_t TICKS_PER_HOUR    = HOUR_MINUTES    * TICKS_PER_MINUTE;
   static const count_t TICKS_PER_DAY     = DAY_HOURS       * TICKS_PER_HOUR;
   static const count_t TICKS_PER_YEAR    = YEAR_DAYS       * TICKS_PER_DAY;
   static const count_t TICKS_PER_LYEAR   = LYEAR_DAYS      * TICKS_PER_DAY;

public:
   static const std::uint8_t ISO_STR_LEN  = 23;
   // ctor decls
   date_time();
   date_time(const date_time & dt);
   explicit date_time(const sk3l::core::datetime::long_clock::time_point & tp);

   date_time & operator=(const date_time & rhs);

   // static helper conversion methods
   static count_t secs_from_ms(count_t & ticks);
   static count_t mins_from_secs(count_t & secs);
   static count_t hours_from_mins(count_t & mins);
   static count_t days_from_hours(count_t & hours);
   static count_t years_from_days(count_t & days);

   // Methods to retrieve aggregate date & time components.
   count_t get_total_milliseconds() const;
   count_t get_total_seconds()      const;
   count_t get_total_minutes()      const;
   count_t get_total_hours()        const;
   count_t get_total_days()         const;
   count_t get_total_years()        const;

   // Methods to retrieve individual date & time components.
   parts_t get_milliseconds() const;
   parts_t get_seconds()      const;
   parts_t get_minutes()      const;
   parts_t get_hours()        const;
   parts_t get_day()          const;
   parts_t get_month()        const;
   parts_t get_year()         const;

   void get_date_time_parts
   (
      parts_t & year,
      parts_t & month,
      parts_t & day,
      parts_t & hours,
      parts_t & mins,
      parts_t & secs,
      parts_t & ms
   ) const;

   void set_date_time_parts
   (
      parts_t year,
      parts_t month,
      parts_t day,
      parts_t hours,
      parts_t mins,
      parts_t secs,
      parts_t ms
   );

   timepoint_t get_time_point() const;

   template <typename Rep, typename Period=std::ratio<1> >
   const date_time operator+(const std::chrono::duration<Rep,Period> & rhs)
   {
      date_time sum(*this);
      sum += rhs;
      return sum;

   }

   template <typename Rep, typename Period=std::ratio<1> >
   const date_time operator-(const std::chrono::duration<Rep,Period> & rhs)
   {
      date_time sum(*this);
      sum -= rhs;
      return sum;
   }

   template <typename Rep, typename Period=std::ratio<1> >
   date_time & operator+=(const std::chrono::duration<Rep,Period> & rhs)
   {
      this->timestamp_ += rhs;
      return *this;
   }

   template <typename Rep, typename Period=std::ratio<1> >
   date_time & operator-=(const std::chrono::duration<Rep,Period> & rhs)
   {
      this->timestamp_ -= rhs;
      return *this;
   }

   static const date_time now();

   template<typename charT=char, typename traitsT=std::char_traits<charT> >
   std::basic_string<charT,traitsT> format_basic_string
   (
      const std::basic_string<charT,traitsT> & format
   ) const
   {
      std::basic_stringstream<charT,traitsT> result;

      bool gotDelim = false;
      for (auto it = format.begin(); it != format.end(); ++it)
      {
         if (!gotDelim)
         {
            if (*it == '%')
               gotDelim = true;
            else
               result.put(*it);
            continue;
         }

         switch (*it)
         {
            case 'F':
               result << std::setfill(get_fill_char<charT>()) << std::setw(4)
                      << this->get_year() << "-"
                      << std::setfill(get_fill_char<charT>()) << std::setw(2)
                      << this->get_month() << "-"
                      << std::setfill(get_fill_char<charT>()) << std::setw(2)
                      << this->get_day();
            break;

            case 'Y':
               result << this->get_year();
            break;

            case 'm':
               result << std::setfill(get_fill_char<charT>()) << std::setw(2)
                      << this->get_month();
            break;

            case 'd':
               result << std::setfill(get_fill_char<charT>()) << std::setw(2)
                      << this->get_day();
            break;

            case 'T':
               result << std::setfill(get_fill_char<charT>())  << std::setw(2)
                      << this->get_hours()   << ":"
                      << std::setfill(get_fill_char<charT>())  << std::setw(2)
                      << this->get_minutes() << ":"
                      << std::setfill(get_fill_char<charT>())  << std::setw(2)
                      << this->get_seconds() << "."            << std::setw(3)
                      << this->get_milliseconds();
            break;

            case 'H':
               result << std::setfill(get_fill_char<charT>())  << std::setw(2)
                      << this->get_hours();
            break;

            case 'M':
               result << std::setfill(get_fill_char<charT>())  << std::setw(2)
                      << this->get_minutes();
            break;

            case 'S':
               result << std::setfill(get_fill_char<charT>())  << std::setw(2)
                      << this->get_seconds();
            break;
        }
         gotDelim = false;
      }

      return result.str();
   }

   template<typename charT=char, typename traitsT=std::char_traits<charT> >
   std::basic_string<charT,traitsT> format_basic_string(const charT * fmt) const
   {
      return this->format_basic_string<charT,traitsT>(std::basic_string<charT,traitsT>(fmt));
   }

   //
   // string formatting convenience methods
   std::string format_str(const std::string & fmt) const
   {
      return this->format_basic_string<char>(fmt);
   }

   std::string format_str(const char * fmt) const
   {
      return this->format_str(std::string(fmt));
   }

   std::string format_iso_date_str() const
   {
      return this->format_basic_string<char>("%F");
   }

   std::string format_iso_time_str() const
   {
      return this->format_basic_string<char>("%T");
   }

   //
   // wstring formatting convenience methods
   std::wstring format_wstr(const std::wstring & fmt) const
   {
      return this->format_basic_string<wchar_t>(fmt);
   }

   std::wstring format_wstr(const wchar_t * fmt) const
   {
      return this->format_wstr(std::wstring(fmt));
   }

   std::wstring format_iso_date_wstr() const
   {
      return this->format_basic_string<wchar_t>(L"%F");
   }

   std::wstring format_iso_time_wstr() const
   {
      return this->format_basic_string<wchar_t>(L"%T");
   }

   template<typename charT=char, typename traitsT=std::char_traits<charT> >
   static date_time parse
   (
      const std::basic_string<charT> & dt,
      const std::basic_string<charT> & format
   )
   {

      using string_t = std::basic_string<charT,traitsT>;
      using string_util = core::text::string_util;
      using num_t = std::uint32_t;

      string_t tmp(dt);
      date_time rv;

      std::uint32_t year, month, day;
      std::uint32_t hours, mins, secs, ms;

      bool gotDelim = false;
      for (auto it = format.begin(); it != format.end(); ++it)
      {
         if (!gotDelim)
         {
            if (*it == '%')
               gotDelim = true;
            continue;
         }

         switch (*it)
         {
            case 'F':
               if (tmp.length() < 10)
                  throw std::invalid_argument
                  (
                     string_t("Date string '") + tmp +
                     string_t("' does not match format '") + format + "'."
                  );
               year = string_util::string_to_numeric<num_t>(tmp.substr(0, 4));
               month= string_util::string_to_numeric<num_t>(tmp.substr(5, 2));
               day  = string_util::string_to_numeric<num_t>(tmp.substr(8, 2));

               tmp.erase(0, 11);
            break;

            case 'T':
            if (tmp.length() < 12)
                  throw std::invalid_argument
                  (
                     string_t("Time string '") + tmp +
                     string_t("' does not match format '") + format + "'."
                  );

               hours= string_util::string_to_numeric<num_t>(tmp.substr(0, 2));
               mins = string_util::string_to_numeric<num_t>(tmp.substr(3, 2));
               secs = string_util::string_to_numeric<num_t>(tmp.substr(6, 2));
               ms   = string_util::string_to_numeric<num_t>(tmp.substr(9, 3));

               tmp = tmp.substr(0, 12);

            break;
         }
         gotDelim = false;
      }

      rv.set_date_time_parts(year, month, day, hours, mins, secs, ms);

      return rv;
   }
};

template<typename charT=char, typename traitsT=std::char_traits<charT> >
std::basic_istream<charT,traitsT> & operator>>
(
   std::basic_istream<charT,traitsT> & is,
   date_time & dt
)
{
   charT str[date_time::ISO_STR_LEN+1];
   is.get(str, date_time::ISO_STR_LEN+1);
   dt =
      date_time::parse
      (
         std::basic_string<charT,traitsT>(str),
         std::basic_string<charT,traitsT>("%FT%T")
      );
   return is;
}

template<typename charT=char, typename charTraits=std::char_traits<charT> >
std::basic_ostream<charT,charTraits> & operator<<
(
   std::basic_ostream<charT,charTraits> & os,
   const date_time & dt
)
{
   os << dt.format_basic_string<charT,charTraits>("%FT%T");
   return os;
}

}
}
}
#endif // DATE_TIME_H
