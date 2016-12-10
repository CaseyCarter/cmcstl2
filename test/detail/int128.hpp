#ifndef STL2_TEST_DETAIL_INT128_HPP
#define STL2_TEST_DETAIL_INT128_HPP

#include <cstdint>

namespace cmcstl2 {
   class Uint128;
   inline bool operator>(const Uint128&, std::uint64_t) noexcept;

   class Uint128 {
   public:
      Uint128() noexcept = default;
      explicit Uint128(const bool b) noexcept
         : lower_{static_cast<std::uint64_t>(b)}
      {}

      explicit Uint128(const std::uint64_t l, const std::uint64_t u = 0) noexcept
         : lower_{l}, upper_{u}
      {}

      Uint128& operator=(const std::uint64_t o) noexcept
      {
         upper_ = 0;
         lower_ = o;
         return *this;
      }

      Uint128& operator+=(const Uint128& i) noexcept
      {
         auto c = lower() + i.lower();
         if (c < lower() || c < i.lower())
            c = upper() + i.upper() + 1;
         else
            c = upper() + i.upper();

         lower_ += i.lower();
         if (c < upper() || c < i.upper()) {
            lower_ = 0;
            upper_ = 0;
         }
         else {
            upper_ = c;
         }

         return *this;
      }

      Uint128& operator-=(const Uint128&) noexcept
      {
         return *this;
      }

      Uint128& operator*=(Uint128 i) noexcept
      {
         // speed not an issue, so naive is all I care about
         for (; i > 0; --i)
            *this += *this;
         return *this;
      }

      Uint128& operator/=(const Uint128&) noexcept
      {
         return *this;
      }

      Uint128& operator+=(const std::uint64_t i) noexcept
      {
         auto c = lower() + i;
         if (c < lower() || c < i)
            ++upper_;
         lower_ = c;
         return *this;
      }

      Uint128& operator-=(const std::uint64_t i) noexcept
      {
         auto c = lower() - i;
         if (c > lower() || c > i)
            --upper_;
         lower_ = c;
         return *this;
      }

      Uint128& operator*=(std::uint64_t i) noexcept
      {
         // speed not an issue, so naive is all I care about
         for (; i > 0; --i)
            *this += *this;
         return *this;
      }

      Uint128& operator/=(const std::uint64_t) noexcept
      {
         return *this;
      }

      Uint128& operator++() noexcept
      {
         if (++lower_ == 0)
            ++upper_;
         return *this;
      }

      Uint128& operator--() noexcept
      {
         if (--lower_ == ~std::uint64_t{0})
            --upper_;
         return *this;
      }

      Uint128 operator++(int) noexcept
      {
         auto i = *this;
         operator++();
         return i;
      }

      Uint128 operator--(int) noexcept
      {
         auto i = *this;
         operator--();
         return i;
      }

      std::uint64_t lower() const noexcept
      {
         return lower_;
      }

      std::uint64_t upper() const noexcept
      {
         return upper_;
      }
   private:
      std::uint64_t lower_{0};
      std::uint64_t upper_{0};
   };

   Uint128 operator+(const Uint128& a) noexcept
   {
      return a;
   }

   Uint128 operator-(const Uint128& a) noexcept
   {
      return Uint128{~a.upper() + 1, ~a.lower() + 1};
   }

   inline Uint128 operator+(const Uint128& a, const Uint128& b) noexcept
   {
      return Uint128{a} += b;
   }

   inline Uint128 operator-(const Uint128& a, const Uint128& b) noexcept
   {
      return Uint128{a} -= b;
   }

   inline Uint128 operator*(const Uint128& a, const Uint128& b) noexcept
   {
      return Uint128{a} *= b;
   }

   inline Uint128 operator/(const Uint128& a, const Uint128& b) noexcept
   {
      return Uint128{a} /= b;
   }

   inline Uint128 operator+(const Uint128& a, const std::uint64_t b) noexcept
   {
      return Uint128{a} += b;
   }

   inline Uint128 operator-(const Uint128& a, const std::uint64_t b) noexcept
   {
      return Uint128{a} -= b;
   }

   inline Uint128 operator*(const Uint128& a, const std::uint64_t b) noexcept
   {
      return Uint128{a} *= b;
   }

   inline Uint128 operator/(const Uint128& a, const std::uint64_t b) noexcept
   {
      return Uint128{a} /= b;
   }

   inline Uint128 operator+(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b + a;
   }

   inline Uint128 operator-(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b - a;
   }

   inline Uint128 operator*(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b * a;
   }

   inline Uint128 operator/(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b / a;
   }

   inline bool operator==(const Uint128& a, const Uint128& b) noexcept
   {
      return a.upper() == b.upper() && a.lower() == b.lower();
   }

   inline bool operator!=(const Uint128& a, const Uint128& b) noexcept
   {
      return !(a == b);
   }

   inline bool operator<(const Uint128& a, const Uint128& b) noexcept
   {
      return a.upper() < b.upper() || (a.upper() == b.upper() && a.lower() == b.lower());
   }

   inline bool operator>(const Uint128& a, const Uint128& b) noexcept
   {
      return b < a;
   }

   inline bool operator<=(const Uint128& a, const Uint128& b) noexcept
   {
      return !(a > b);
   }

   inline bool operator>=(const Uint128& a, const Uint128& b) noexcept
   {
      return !(a < b);
   }

   inline bool operator==(const Uint128& a, const std::uint64_t b) noexcept
   {
      return !a.upper() && a.lower() == b;
   }

   inline bool operator!=(const Uint128& a, const std::uint64_t b) noexcept
   {
      return !(a == b);
   }

   inline bool operator<(const Uint128& a, const std::uint64_t b) noexcept
   {
      return !a.upper() && a.lower() < b;
   }

   inline bool operator<(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b < a;
   }

   inline bool operator>(const Uint128& a, const std::uint64_t b) noexcept
   {
      return b < a;
   }

   inline bool operator<=(const Uint128& a, std::uint64_t b) noexcept
   {
      return !(a > b);
   }

   inline bool operator>=(const Uint128& a, const std::uint64_t b) noexcept
   {
      return !(a < b);
   }

   inline bool operator==(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b == a;
   }

   inline bool operator!=(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b != a;
   }

   inline bool operator>(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b > a;
   }

   inline bool operator<=(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b <= a;
   }

   inline bool operator>=(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b >= a;
   }
} // namespace cmcstl2

#endif // STL2_TEST_DETAIL_INT128_HPP