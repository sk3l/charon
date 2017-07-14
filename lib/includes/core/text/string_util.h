#ifndef STRING_UTIL_H 
#define STRING_UTIL_H

#include <algorithm>
#include <cmath>
#include <cctype>
#include <cinttypes>
#include <cwctype>
#include <exception>
#include <iterator>
#include <limits>
#include <memory>
#include <string>
#include <sstream>
#include <typeinfo>
#include <vector>

namespace sk3l
{
namespace core
{
namespace text 
{

template<typename charT, typename traits=std::char_traits<charT> >
class basic_string_util
{
   public:
      using string_t = std::basic_string<charT,traits>;
      
      static string_t to_upper(const string_t & s)
      {
         string_t cpy;
         std::for_each
         (
            s.begin(),
            s.end(),
            [&cpy](charT c){cpy.push_back(std::toupper(c));}
         );

         return cpy;
      }

      static string_t to_lower(const string_t & s)
      {
         string_t cpy;
         std::for_each
         (
            s.begin(),
            s.end(),
            [&cpy](charT c){cpy.push_back(std::tolower(c));}
         );

         return cpy;
      }

      static string_t strip_ws(const string_t & s)
      {
         string_t cpy;
         std::copy_if
         (
            s.begin(),
            s.end(),
            std::back_inserter(cpy),
            [](const charT& c){return !std::iswspace(c);}
         );

         return cpy;
      }

      using string_list_t = std::vector<string_t>;

      static string_list_t split(const string_t & s, charT delim)
      {
         string_list_t sl;

         auto beg = s.begin(),
              end = s.end();

         while (beg != end)
         {
            string_t nxt_str;
            auto nxt = std::find(beg, end, delim);
 
            std::copy(beg, nxt, std::back_inserter(nxt_str));
            
            if (nxt_str.length() > 0) 
               sl.push_back(nxt_str);
            beg = nxt;

            if (beg != end)
               ++beg;         
         }

         return sl;
      }

      static string_t reverse(const string_t & str)
      {
         string_t rev;
         for (auto r = str.rbegin(); r != str.rend(); ++r)
            rev.push_back(*r);

         return rev;
      }

      template<typename T>
      static T string_to_numeric(const string_t & str)
      {
         T rv = {0};

         if 
         (
            (str.size() < 1) || 
            (str.size() > (std::numeric_limits<T>::digits10)+1)
         )
         {
            std::string err = "String value '";
            err += str + "' has incorrect numer of digits.";
            throw std::out_of_range(err);
         }

         size_t expnt = 0;
         for (int i = str.size()-1; i >= 0; --i)
         {
            if (str[i] < '0' || str[i] > '9')
            {
               std::string err = "String value '";
               err += str + "' is not a valid numeric.";
                throw std::range_error(err);
            }

            T char_val = str[i] - '0';
            T base_val = pow(10, expnt++);

            if (char_val > (std::numeric_limits<T>::max() / base_val))
            {
               std::string err = "String value '";
               err += str + "' is out of range.";
               throw std::overflow_error(err);
            }

            T dec = char_val * base_val;
            T sum = rv + dec;

            if ((sum < rv) && (sum < dec))
            {
               std::string err = "String value '";
               err += str + "' is out of range.";
               throw std::overflow_error(err); // overflow, throw
            }
            rv = sum;
         }
         
         return rv; 
      }

      template<typename T>
      static string_t numeric_to_string(T t)
      {
         if (!std::is_integral<T>::value)
            throw std::runtime_error("Value is not integral.");

         string_t str = "";
         do
         {
            charT c = '0' +  (t % 10);
            str.push_back(c); 
            t = t  / 10; 
         }
         while (t != 0);

         return basic_string_util<charT>::reverse(str);
      }

      template<typename T>
      static T parse(const string_t & s)
      {
         T t;
         std::stringstream ss(s);
         ss >> t;

         if (ss.fail())
            throw std::runtime_error("Couldn't parse object from string.");
         return t;  
      }

      template<typename T>
      static bool try_parse(const string_t & str, T & t)
      {
         std::stringstream ss(str);
         ss >> t;
         return !ss.fail();
      }
 
};

using string_util = basic_string_util<char>;
using wstring_util= basic_string_util<wchar_t>;

// Full specialization of parse on std::string
template<>
template<>
inline std::string string_util::parse
(
   const std::string & str
)
{
   return str;
}

// Full specialization of parse on std::wstring
template<>
template<>
inline std::wstring wstring_util::parse
(
   const std::wstring & str
)
{
   return str;
}


}
}
}

#endif
