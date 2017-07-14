#ifndef NULLABLE_H
#define NULLABLE_H

#include <exception>
#include <utility>


namespace sk3l {
namespace core {

template <typename T>
class nullable
{
private:
   T * t_;
public:
   nullable()
      : t_(nullptr)
   {}

   explicit nullable(const T & t)
      : t_(new T(t))
   {}

   nullable(const nullable & n)
      : t_(n.t_ ? new T(*n.t_) : nullptr)
   {}

   nullable(nullable && n)
   {
      this->t_ = n.t_;
      n.t_ = nullptr;
   }

   void swap(nullable & rhs)
   {
      std::swap(this->t_, rhs.t_);
   }


   nullable & operator=(nullable n)
   {
      if (this != &n)
      {
         swap(n);
      }

      return *this;
   }

   bool has_value() const
   {
      return this->t_ != nullptr;
   }

   operator bool() const
   {
      return this->has_value();
   }

   T get()
   {
      if (this->t_)
         return *(this->t_);
      else
         throw std::exception();
   }

   bool try_get(T & t)
   {
      bool hasValue = this->has_value();

      if (hasValue)
         t = *(this->t_);
      return hasValue;
   }

   void set(const T & t)
   {
      nullable temp(t);
      std::swap(*this, temp);
   }

   void set(T * t)
   {
      nullable temp(&t);
      std::swap(*this, temp);
   }

   ~nullable()
   {
      delete t_;
      t_ = nullptr;
   }
};

}
}
#endif // NULLABLE_H
