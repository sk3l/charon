#ifndef STREAM_PARSER_H
#define STREAM_PARSER_H

#include <algorithm>
#include <cctype>
#include <cstring>
#include <iomanip>
#include <istream>
#include <limits>
#include <memory>
#include <string>

namespace sk3l
{
namespace core
{
namespace io
{

template<typename charT, typename traits=std::char_traits<charT> >
class stream_parser
{
   private:
      using string_t = std::basic_string<charT,traits>;

      std::istream & is_;
      string_t       delims_;
      bool           matchAll_;
      bool           keepDelim_;
      bool           skipWhitespace_;
      charT          lastDelim_;

      string_t match_any()
      {
         string_t nxt;
         charT c;
         bool leading = true;

         while (this->is_ >> c)
         {
            if (this->delims_.find(c) != string_t::npos)
            {
               lastDelim_ = c;
               break;
            }

            if (this->skipWhitespace_ && leading)
            {
               if (std::isspace(c))
                  continue;
               else
                  leading = false;
            }

            nxt.push_back(c);
         }

         if (this->is_ && this->keepDelim_)
            this->is_.putback(c);

         return nxt;
      }

      string_t match_all()
      {
         std::unique_ptr<charT> buff(new charT[this->delims_.size()+1]);
         std::memset(buff.get(), 0, this->delims_.size()+1);

         string_t nxt;
         charT c;
         bool leading = true;

         while (this->is_ >> c)
         {
            if (c != this->delims_.front())
            {
               if (this->skipWhitespace_ && leading)
               {
                  if (std::isspace(c))
                     continue;
                  else
                     leading = false;
               }

               nxt.push_back(c);
            }
            else
            {
               bool match = true;
               buff.get()[0] = c;
               size_t end = this->delims_.size();
               for (size_t i = 1; (i < end) && (this->is_ >> c); ++i)
               {
                  buff.get()[i] = c;
                  if (c != this->delims_[i])
                  {
                     match = false;
                     nxt.append(buff.get());
                     std::memset(buff.get(), 0, this->delims_.size()+1);
                     break;
                  }
               }

               if (match)
               {
                  if (this->keepDelim_)
                     for (auto it = this->delims_.rbegin(); it != this->delims_.rend(); ++it)
                        this->is_.putback(*it);
                  break;
               }

            }
         }

         return nxt;
      }

   public:

      explicit stream_parser(std::istream & is)
         : is_(is),
           delims_(""),
           //delims_(std::numeric_limits<charT>::max(), false),
           matchAll_(false),
           keepDelim_(false),
           skipWhitespace_(false),
           lastDelim_(0)
      {
         this->is_.unsetf(std::ios_base::skipws);
      }

      stream_parser
      (
         std::istream & is,
         const string_t & delims,
         bool matchAll =false,
         bool keepDelim=false,
         bool skipWhitespace=false
      )
         : is_(is),
           delims_(delims),
           //delims_(std::numeric_limits<charT>::max(), false),
           matchAll_(matchAll),
           keepDelim_(keepDelim),
           skipWhitespace_(skipWhitespace),
           lastDelim_(0)
      {
         this->is_.unsetf(std::ios_base::skipws);
         //this->populate_delims(delims);
      }
/*
      stream_parser
      (
         std::istream & is,
         const charT * delims,
         bool matchAll=false,
         bool keepDelim=false
      )
         : is_(is),
           delims_(delims),
           //delims_(std::numeric_limits<charT>::max(), false),
           matchAll_(matchAll),
           keepDelim_(keepDelim),
           lastDelim_(0)
      {
         //this->populate_delims(string_t(delims));
      }
*/
      void set_delims(const string_t & delims)
      {
         this->delims_ = delims;
         //this->populate_delims(delims);
      }

      void set_match_all(bool match)
      {
         this->matchAll_ = match;
      }

      void set_keep_delims(bool keep)
      {
         this->keepDelim_ = keep;
      }

      charT get_last_delim() const
      {
         return this->lastDelim_;
      }

      string_t next_match()
      {
         if (!this->matchAll_)
            return this->match_any();
         else
            return this->match_all();
      }

      string_t next_word()
      {
         string_t nxt;
         charT c;
         while (this->is_ >> c && std::isalnum(c))
            nxt.push_back(c);

         return nxt;
      }

      void discard
      (
         size_t cnt,
         const string_t & delims = "",
         bool keepDelims = true
      )
      {
         this->set_delims(delims);
         charT c;

         for (size_t i = 0;
              i < cnt && this->is_ >> c;
              ++i)
         {
            //if (delims_[traits::to_int_type(c)])
            if (this->delims_.find(c) != string_t::npos)
            {
               lastDelim_ = c;
               if (keepDelims)
                  this->is_.putback(c);
               break;
            }
         }
      }

      void discard(const string_t & delims, bool keepDelims = true)
      {
         this->discard
         (
            std::numeric_limits<std::streamsize>::max(),
            delims,
            keepDelims
         );
      }

      string_t peek(size_t count)
      {
         string_t peek;
         charT c;

         for (size_t i = 0;
              i < count && this->is_ >> c;
              ++i)
         {
            peek.push_back(c);
         }

         for (int i = peek.size()-1; i >= 0; --i)
            this->is_.putback(peek[i]);

         return peek;
      };

      stream_parser & operator>>(string_t & nxt)
      {
         nxt = this->next_match();
         return *this;
      }

      void skipws()
      {
         //this->is_ >> std::skipws;
         this->skipWhitespace_ = true;
      }

      void noskipws()
      {
         //this->is_ >> std::noskipws;
         this->skipWhitespace_ = false;
      }
};

   // I/O manipulator for assigning stream parser delimeters.
   template<typename charT, typename traits=std::char_traits<charT> >
   class basic_setdelims
   {
      private:
         using string_t = std::basic_string<charT,traits>;
         using sp_t = stream_parser<charT,traits>;
         const charT * delims_;

      public:
         explicit basic_setdelims(const charT * delims)
         : delims_(delims)
         {}

         sp_t & operator()(sp_t & sp) const
         {
            sp.set_delims(string_t(this->delims_));
            return sp;
         }

  };

   template<typename charT, typename traits=std::char_traits<charT> >
   stream_parser<charT,traits> & operator>>
   (
      stream_parser<charT,traits> & sp,
      const basic_setdelims<charT,traits> & sd
   )
   {
      return sd(sp);
   }

   using setdelims = basic_setdelims<char>;
   using wsetdelims = basic_setdelims<wchar_t>;

   // I/O manipulator for controllling stream parser keep delim.
   template<typename charT, typename traits=std::char_traits<charT> >
   class basic_keepdelims
   {
      private:
         using string_t = std::basic_string<charT,traits>;
         using sp_t = stream_parser<charT,traits>;
         bool keep_;

      public:
         explicit basic_keepdelims(bool b)
         : keep_(b)
         {}

         sp_t & operator()(sp_t & sp) const
         {
            sp.set_keep_delims(this->keep_);
            return sp;
         }

  };

   template<typename charT, typename traits=std::char_traits<charT> >
   stream_parser<charT,traits> & operator>>
   (
      stream_parser<charT,traits> & sp,
      const basic_keepdelims<charT,traits> & sd
   )
   {
      return sd(sp);
   }

   using keepdelims = basic_keepdelims<char>;
   using wkeepdelims = basic_keepdelims<wchar_t>;

   // I/O manipulator for controllling stream parser skip whitespace.
   template<typename charT, typename traits=std::char_traits<charT> >
   stream_parser<charT,traits> & skipws(stream_parser<charT,traits> & sp )
   {
      sp.skipws();
      return sp;
   }

   template<typename charT, typename traits=std::char_traits<charT> >
   stream_parser<charT,traits> & noskipws(stream_parser<charT,traits> & sp )
   {
      sp.noskipws();
      return sp;
   }

   // I/O manipulator for controllling stream parser match all.
   template<typename charT, typename traits=std::char_traits<charT> >
   class basic_match_all
   {
      private:
         using string_t = std::basic_string<charT,traits>;
         using sp_t = stream_parser<charT,traits>;
         bool matchAll_;

      public:
         explicit basic_match_all(bool b)
         : matchAll_(b)
         {}

         sp_t & operator()(sp_t & sp) const
         {
            sp.set_match_all(this->matchAll_);
            return sp;
         }

  };

   template<typename charT, typename traits=std::char_traits<charT> >
   stream_parser<charT,traits> & operator>>
   (
      stream_parser<charT,traits> & sp,
      const basic_match_all<charT,traits> & sd
   )
   {
      return sd(sp);
   }

   using matchall = basic_match_all<char>;
   using wmatchall= basic_match_all<wchar_t>;

   // Stream operator for applying a function object to a stream parser
   template<typename charT, typename traits=std::char_traits<charT> >
   stream_parser<charT,traits> & operator>>
   (
      stream_parser<charT,traits> & sp,
      stream_parser<charT,traits> & (*func)(stream_parser<charT,traits>&)
   )
   {
      func(sp);
      return sp;
   }


}

}

}


#endif
