#ifndef STL2_TEST_DETAIL_INT128_HPP
#define STL2_TEST_DETAIL_INT128_HPP

#include <cstdint>
#include <stl2/detail/fwd.hpp>

namespace cmcstl2 {
   class Uint128;
   constexpr Uint128 operator+(const Uint128 i) noexcept;
   constexpr Uint128 operator-(const Uint128 i) noexcept;
   constexpr Uint128 operator~(const Uint128 i) noexcept;

   constexpr Uint128 operator+(Uint128 a, const Uint128 b) noexcept;
   constexpr Uint128 operator-(Uint128 a, const Uint128 b) noexcept;
   Uint128 operator*(const Uint128, const Uint128) noexcept;
   Uint128 operator/(const Uint128, const Uint128) noexcept;
   Uint128 operator%(const Uint128, const Uint128) noexcept;
   constexpr Uint128 operator+(Uint128 a, const std::uint64_t b) noexcept;
   constexpr Uint128 operator-(Uint128 a, const std::uint64_t b) noexcept;
   Uint128 operator*(const Uint128, std::uint64_t) noexcept;
   Uint128 operator/(const Uint128, std::uint64_t) noexcept;
   Uint128 operator%(const Uint128, std::uint64_t) noexcept;
   Uint128 operator+(std::uint64_t, const Uint128) noexcept;
   Uint128 operator-(std::uint64_t, const Uint128) noexcept;
   Uint128 operator*(std::uint64_t, const Uint128) noexcept;
   Uint128 operator/(std::uint64_t, const Uint128) noexcept;
   Uint128 operator%(std::uint64_t, const Uint128) noexcept;
   Uint128 operator&(const Uint128, std::uint64_t) noexcept;
   Uint128 operator|(const Uint128, std::uint64_t) noexcept;
   Uint128 operator^(const Uint128, std::uint64_t) noexcept;
   Uint128 operator<<(const Uint128, std::uint64_t) noexcept;
   Uint128 operator>>(const Uint128, std::uint64_t) noexcept;

   constexpr bool operator==(const Uint128, const Uint128) noexcept;
   constexpr bool operator!=(const Uint128, const Uint128) noexcept;
   constexpr bool operator<(const Uint128 a, const Uint128 b) noexcept;
   constexpr bool operator>(Uint128, Uint128) noexcept;
   constexpr bool operator<=(Uint128, Uint128) noexcept;
   constexpr bool operator>=(Uint128, Uint128) noexcept;
   
   constexpr bool operator==(Uint128, std::uint64_t) noexcept;
   constexpr bool operator!=(Uint128, std::uint64_t) noexcept;
   constexpr bool operator<(Uint128 a, std::uint64_t) noexcept;
   constexpr bool operator>(Uint128, std::uint64_t) noexcept;
   constexpr bool operator<=(Uint128, std::uint64_t) noexcept;
   constexpr bool operator>=(Uint128, std::uint64_t) noexcept;

   constexpr bool operator==(std::uint64_t, Uint128) noexcept;
   constexpr bool operator!=(std::uint64_t, Uint128) noexcept;
   constexpr bool operator<(std::uint64_t, Uint128) noexcept;
   constexpr bool operator>(std::uint64_t, Uint128) noexcept;
   constexpr bool operator<=(std::uint64_t, Uint128) noexcept;
   constexpr bool operator>=(std::uint64_t, Uint128) noexcept;

   class Uint128 {
   public:
      constexpr Uint128() = default;
      constexpr Uint128(std::uint64_t lower, std::uint64_t upper = 0) noexcept
         : lower_{lower}, upper_{upper}
      {}
      Uint128& operator=(std::uint64_t o) noexcept;

      constexpr Uint128& operator+=(const Uint128 i) noexcept
      {
         // TODO: factorise into constexpr lambda instead of private member function
         lower_ = add_with_carry(lower(), i.lower(), upper_);
         upper_ = add_with_carry(upper(), i.upper(), lower_);
         return *this;
      }

      constexpr Uint128& operator-=(const Uint128 i) noexcept
      {
         return *this += -i;
      }

      Uint128& operator*=(const Uint128) noexcept;
      Uint128& operator/=(const Uint128) noexcept;
      Uint128& operator%=(const Uint128) noexcept;

      constexpr Uint128& operator+=(const std::uint64_t i) noexcept
      {
         return *this += Uint128{i};
      }

      constexpr Uint128& operator-=(const std::uint64_t i) noexcept
      {
         return *this -= Uint128{i};
      }

      Uint128& operator*=(std::uint64_t) noexcept;
      Uint128& operator/=(std::uint64_t) noexcept;
      Uint128& operator%=(std::uint64_t) noexcept;

      Uint128& operator&=(std::uint64_t) noexcept;
      Uint128& operator|=(std::uint64_t) noexcept;
      Uint128& operator^=(std::uint64_t) noexcept;
      Uint128& operator<<=(std::uint64_t) noexcept;
      Uint128& operator>>=(std::uint64_t) noexcept;

      Uint128& operator++() noexcept;
      Uint128& operator--() noexcept;

      Uint128 operator++(int) noexcept;
      Uint128 operator--(int) noexcept;

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

      constexpr std::uint64_t add_with_carry(const std::uint64_t a, const std::uint64_t b, std::uint64_t& awc)
      {
         const auto c = a + b;
         if (c < a)
            ++awc;
         return a;
      }
   };

   constexpr Uint128 operator+(const Uint128 i) noexcept
   {
      return i;
   }

   constexpr Uint128 operator-(const Uint128 i) noexcept
   {
      return Uint128{~i.upper(), ~i.lower()} + 1;
   }

   constexpr Uint128 operator~(const Uint128 i) noexcept
   {
      return {~i.upper(), ~i.lower()};
   }

   constexpr Uint128 operator+(Uint128 a, const Uint128 b) noexcept
   {
      return a += b;
   }

   constexpr Uint128 operator-(Uint128 a, const Uint128 b) noexcept
   {
      return a -= b;
   }

   Uint128 operator*(const Uint128, const Uint128) noexcept;
   Uint128 operator/(const Uint128, const Uint128) noexcept;
   Uint128 operator%(const Uint128, const Uint128) noexcept;

   constexpr Uint128 operator+(Uint128 a, const std::uint64_t b) noexcept
   {
      return a += b;
   }

   constexpr Uint128 operator-(Uint128 a, const std::uint64_t b) noexcept
   {
      return a -= b;
   }

   Uint128 operator*(const Uint128, std::uint64_t) noexcept;
   Uint128 operator/(const Uint128, std::uint64_t) noexcept;
   Uint128 operator%(const Uint128, std::uint64_t) noexcept;
   Uint128 operator+(std::uint64_t, const Uint128) noexcept;
   Uint128 operator-(std::uint64_t, const Uint128) noexcept;
   Uint128 operator*(std::uint64_t, const Uint128) noexcept;
   Uint128 operator/(std::uint64_t, const Uint128) noexcept;
   Uint128 operator%(std::uint64_t, const Uint128) noexcept;
   Uint128 operator&(const Uint128, std::uint64_t) noexcept;
   Uint128 operator|(const Uint128, std::uint64_t) noexcept;
   Uint128 operator^(const Uint128, std::uint64_t) noexcept;
   Uint128 operator<<(const Uint128, std::uint64_t) noexcept;
   Uint128 operator>>(const Uint128, std::uint64_t) noexcept;

   constexpr bool operator==(const Uint128 a, const Uint128 b) noexcept
   {
      return a.lower() == b.lower() && a.upper() == b.upper();
   }

   constexpr bool operator!=(const Uint128 a, const Uint128 b) noexcept
   {
      return !(a == b);
   }

   constexpr bool operator<(const Uint128 a, const Uint128 b) noexcept
   {
      return a.upper() < b.upper() || (a.upper() == b.upper() && a.lower() < b.lower());
   }

   constexpr bool operator>(const Uint128 a, const Uint128 b) noexcept
   {
      return b < a;
   }

   constexpr bool operator<=(const Uint128 a, const Uint128 b) noexcept
   {
      return !(a > b);
   }

   constexpr bool operator>=(const Uint128 a, const Uint128 b) noexcept
   {
      return !(a < b);
   }

   constexpr bool operator==(const Uint128 a, const std::uint64_t b) noexcept
   {
      return !a.upper() && a.lower() == b;
   }
   
   constexpr bool operator!=(const Uint128 a, const std::uint64_t b) noexcept
   {
      return !(a == b);
   }

   constexpr bool operator<(const Uint128 a, const std::uint64_t b) noexcept
   {
      return a < Uint128{b};
   }

   constexpr bool operator>(const Uint128 a, const std::uint64_t b) noexcept
   {
      return Uint128{b} < a;
   }

   constexpr bool operator<=(const Uint128 a, const std::uint64_t b) noexcept
   {
      return !(a > Uint128{b});
   }

   constexpr bool operator>=(const Uint128 a, const std::uint64_t b) noexcept
   {
      return !(a < Uint128{b});
   }

   constexpr bool operator==(const std::uint64_t a, const Uint128 b) noexcept
   {
      return (b == a);
   }

   constexpr bool operator!=(const std::uint64_t a, const Uint128 b) noexcept
   {
      return !(a == b);
   }

   constexpr bool operator<(const std::uint64_t a, const Uint128 b) noexcept
   {
      return b > a;
   }

   constexpr bool operator>(const std::uint64_t a, const Uint128 b) noexcept
   {
      return b < a;
   }

   constexpr bool operator<=(const std::uint64_t a, const Uint128 b) noexcept
   {
      return !(b > a);
   }

   constexpr bool operator>=(const std::uint64_t a, const Uint128 b) noexcept
   {
      return !(b < a);
   }
} // namespace cmcstl2

#endif // STL2_TEST_DETAIL_INT128_HPP