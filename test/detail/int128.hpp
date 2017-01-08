#ifndef STL2_TEST_DETAIL_INT128_HPP
#define STL2_TEST_DETAIL_INT128_HPP

#include <cstdint>
#include <stl2/detail/fwd.hpp>

namespace cmcstl2 {
   class Uint128;
   constexpr bool operator==(const Uint128& a, const Uint128& b) noexcept;
   constexpr bool operator!=(const Uint128& a, const Uint128& b) noexcept;
   constexpr bool operator<(const Uint128& a, const Uint128& b) noexcept;
   constexpr bool operator>(const Uint128& a, const Uint128& b) noexcept;
   constexpr bool operator<=(const Uint128& a, const Uint128& b) noexcept;
   constexpr bool operator>=(const Uint128& a, const Uint128& b) noexcept;
   constexpr bool operator==(const Uint128& a, const std::uint64_t b) noexcept;
   constexpr bool operator!=(const Uint128& a, const std::uint64_t b) noexcept;
   constexpr bool operator<(const Uint128& a, const std::uint64_t b) noexcept;
   constexpr bool operator<(const std::uint64_t a, const Uint128& b) noexcept;
   constexpr bool operator>(const Uint128& a, const std::uint64_t b) noexcept;
   constexpr bool operator<=(const Uint128& a, std::uint64_t b) noexcept;
   constexpr bool operator>=(const Uint128& a, const std::uint64_t b) noexcept;
   constexpr bool operator==(const std::uint64_t a, const Uint128& b) noexcept;
   constexpr bool operator!=(const std::uint64_t a, const Uint128& b) noexcept;
   constexpr bool operator>(const std::uint64_t a, const Uint128& b) noexcept;
   constexpr bool operator<=(const std::uint64_t a, const Uint128& b) noexcept;
   constexpr bool operator>=(const std::uint64_t a, const Uint128& b) noexcept;

   class Uint128 {
   public:
      constexpr Uint128() noexcept = default;
      constexpr Uint128(const __stl2::Integral l, const std::uint64_t u = 0) noexcept
         : lower_{static_cast<std::uint64_t>(l)}, upper_{u}
      {}

      constexpr Uint128& operator=(const std::uint64_t o) noexcept
      {
         upper_ = 0;
         lower_ = o;
         return *this;
      }

      constexpr Uint128& operator+=(const Uint128& i) noexcept
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

      constexpr Uint128& operator-=(const Uint128&) noexcept
      {
         return *this;
      }

      constexpr Uint128& operator*=(Uint128 i) noexcept
      {
         // speed not an issue, so naive is all I care about
         for (; i > 0; --i)
            *this += *this;
         return *this;
      }

      constexpr Uint128& operator/=(const Uint128&) noexcept
      {
         return *this;
      }

      constexpr Uint128& operator%=(const Uint128& i) noexcept
      {
         while (*this > i)
            *this -= i;
         return *this;
      }

      constexpr Uint128& operator+=(const std::uint64_t i) noexcept
      {
         auto c = lower() + i;
         if (c < lower() || c < i)
            ++upper_;
         lower_ = c;
         return *this;
      }

      constexpr Uint128& operator-=(const std::uint64_t i) noexcept
      {
         auto c = lower() - i;
         if (c > lower() || c > i)
            --upper_;
         lower_ = c;
         return *this;
      }

      constexpr Uint128& operator*=(std::uint64_t i) noexcept
      {
         // speed not an issue, so naive is all I care about
         for (const auto a = *this; i > 0; --i)
            *this += a;
         return *this;
      }

      constexpr Uint128& operator/=(const std::uint64_t) noexcept
      {
         return *this;
      }

      constexpr Uint128& operator%=(const std::uint64_t i) noexcept
      {
         while (*this > i)
            *this -= i;
         return *this;
      }

      constexpr Uint128& operator++() noexcept
      {
         if (++lower_ == 0)
            ++upper_;
         return *this;
      }

      constexpr Uint128& operator--() noexcept
      {
         if (--lower_ == ~std::uint64_t{0})
            --upper_;
         return *this;
      }

      constexpr Uint128 operator++(int) noexcept
      {
         auto i = *this;
         operator++();
         return i;
      }

      constexpr Uint128 operator--(int) noexcept
      {
         auto i = *this;
         operator--();
         return i;
      }

      constexpr std::uint64_t lower() const noexcept
      {
         return lower_;
      }

      constexpr std::uint64_t upper() const noexcept
      {
         return upper_;
      }
   private:
      std::uint64_t lower_{0};
      std::uint64_t upper_{0};
   };

   constexpr Uint128 operator+(const Uint128& a) noexcept
   {
      return a;
   }

   constexpr Uint128 operator-(const Uint128& a) noexcept
   {
      return Uint128{~a.upper() + 1, ~a.lower() + 1};
   }

   constexpr Uint128 operator+(const Uint128& a, const Uint128& b) noexcept
   {
      return Uint128{a} += b;
   }

   constexpr Uint128 operator-(const Uint128& a, const Uint128& b) noexcept
   {
      return Uint128{a} -= b;
   }

   constexpr Uint128 operator*(const Uint128& a, const Uint128& b) noexcept
   {
      return Uint128{a} *= b;
   }

   constexpr Uint128 operator/(const Uint128& a, const Uint128& b) noexcept
   {
      return Uint128{a} /= b;
   }

   constexpr Uint128 operator%(const Uint128& a, const Uint128& b) noexcept
   {
      return Uint128{a} %= b;
   }

   constexpr Uint128 operator+(const Uint128& a, const std::uint64_t b) noexcept
   {
      return Uint128{a} += b;
   }

   constexpr Uint128 operator-(const Uint128& a, const std::uint64_t b) noexcept
   {
      return Uint128{a} -= b;
   }

   constexpr Uint128 operator*(const Uint128& a, const std::uint64_t b) noexcept
   {
      return Uint128{a} *= b;
   }

   constexpr Uint128 operator/(const Uint128& a, const std::uint64_t b) noexcept
   {
      return Uint128{a} /= b;
   }

   constexpr Uint128 operator%(const Uint128& a, const std::uint64_t b) noexcept
   {
      return Uint128{a} %= b;
   }

   constexpr Uint128 operator+(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b + a;
   }

   constexpr Uint128 operator-(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b - a;
   }

   constexpr Uint128 operator*(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b * a;
   }

   constexpr Uint128 operator/(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b / a;
   }

   constexpr Uint128 operator%(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b % a;
   }

   constexpr bool operator==(const Uint128& a, const Uint128& b) noexcept
   {
      return a.upper() == b.upper() && a.lower() == b.lower();
   }

   constexpr bool operator!=(const Uint128& a, const Uint128& b) noexcept
   {
      return !(a == b);
   }

   constexpr bool operator<(const Uint128& a, const Uint128& b) noexcept
   {
      return a.upper() < b.upper() || (a.upper() == b.upper() && a.lower() == b.lower());
   }

   constexpr bool operator>(const Uint128& a, const Uint128& b) noexcept
   {
      return b < a;
   }

   constexpr bool operator<=(const Uint128& a, const Uint128& b) noexcept
   {
      return !(a > b);
   }

   constexpr bool operator>=(const Uint128& a, const Uint128& b) noexcept
   {
      return !(a < b);
   }

   constexpr bool operator==(const Uint128& a, const std::uint64_t b) noexcept
   {
      return !a.upper() && a.lower() == b;
   }

   constexpr bool operator!=(const Uint128& a, const std::uint64_t b) noexcept
   {
      return !(a == b);
   }

   constexpr bool operator<(const Uint128& a, const std::uint64_t b) noexcept
   {
      return !a.upper() && a.lower() < b;
   }

   constexpr bool operator<(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b < a;
   }

   constexpr bool operator>(const Uint128& a, const std::uint64_t b) noexcept
   {
      return b < a;
   }

   constexpr bool operator<=(const Uint128& a, std::uint64_t b) noexcept
   {
      return !(a > b);
   }

   constexpr bool operator>=(const Uint128& a, const std::uint64_t b) noexcept
   {
      return !(a < b);
   }

   constexpr bool operator==(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b == a;
   }

   constexpr bool operator!=(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b != a;
   }

   constexpr bool operator>(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b > a;
   }

   constexpr bool operator<=(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b <= a;
   }

   constexpr bool operator>=(const std::uint64_t a, const Uint128& b) noexcept
   {
      return b >= a;
   }
} // namespace cmcstl2

STL2_OPEN_NAMESPACE {
   template <>
   struct common_type<cmcstl2::Uint128, std::uint64_t> {
      using type = cmcstl2::Uint128;
   };

   template <>
   struct common_type<std::uint64_t, cmcstl2::Uint128> {
      using type = cmcstl2::Uint128;
   };
} STL2_CLOSE_NAMESPACE

#endif // STL2_TEST_DETAIL_INT128_HPP