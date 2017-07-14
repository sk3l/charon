#ifndef JULIAN_CALENDAR_H 
#define JULIAN_CALENDAR_H

namespace sk3l{

namespace core{

namespace datetime {

class calendar 
{
   public:

   enum months : short 
   {
      UNKNOWN  = 0,
      JANUARY  = 1,
      FEBRUARY = 2,
      MARCH    = 3,
      APRIL    = 4,
      MAY      = 5,
      JUNE     = 6,
      JULY     = 7,
      AUGUST   = 8,
      SEPTEMBER= 9,
      OCTOBER  = 10,
      NOVEMBER = 11,
      DECEMBER = 12
   };

   using day_of_month = char;

   struct calendar_date
   {

      months         month_;
      day_of_month   day_;

      calendar_date()
         : month_(UNKNOWN), day_(0)
      {}
   
      calendar_date(months m, day_of_month d)
         : month_(m), day_(d)
      {}
   };

   using julian_date = short;

   static bool is_leap_year(long years);
   static long leap_years(long years);

   static calendar_date get_calendar_date(int year, int jday);
   static julian_date get_julian_date(int year, months month, day_of_month day);

   private:
      static const calendar_date jul_to_cal_map_[366];
      static const calendar_date jul_to_lcal_map_[367];

      static const julian_date cal_to_jul_map_[13][32];
      static const julian_date lcal_to_jul_map_[13][32]; 
};


}
}
}

#endif // JULIAN_CALENDAR_H
