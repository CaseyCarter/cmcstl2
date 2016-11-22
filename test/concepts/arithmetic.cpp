// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
//  Copyright Christopher Di Bella 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include "validate.hpp"

#if VALIDATE_RANGES
#elif VALIDATE_STL2
#include <stl2/detail/concepts/arithmetic.hpp>
#endif // VALIDATE_

#include <complex> // TODO: replace with <stl2/detail/complex.hpp>
#include <string>
#include <vector>

namespace ranges = __stl2;

template <ranges::Arithmetic I, ranges::Arithmetic B>
   requires ranges::Arithmetic<I, B>()
void test(I, B)
{
}

int main()
{
   test(1, false);
}

// fundamental types
CONCEPT_ASSERT(models::Arithmetic<bool>);
CONCEPT_ASSERT(models::Arithmetic<bool, bool>);
CONCEPT_ASSERT(models::Arithmetic<bool, bool, bool>);

CONCEPT_ASSERT(models::Arithmetic<char>);
CONCEPT_ASSERT(models::Arithmetic<char, char>);
CONCEPT_ASSERT(models::Arithmetic<char, char, char>);

CONCEPT_ASSERT(models::Arithmetic<signed char>);
CONCEPT_ASSERT(models::Arithmetic<signed char, signed char>);
CONCEPT_ASSERT(models::Arithmetic<signed char, signed char, signed char>);

CONCEPT_ASSERT(models::Arithmetic<unsigned char>);
CONCEPT_ASSERT(models::Arithmetic<unsigned char, unsigned char>);
CONCEPT_ASSERT(models::Arithmetic<unsigned char, unsigned char, unsigned char>);

CONCEPT_ASSERT(models::Arithmetic<short>);
CONCEPT_ASSERT(models::Arithmetic<short, short>);
CONCEPT_ASSERT(models::Arithmetic<short, short, short>);

CONCEPT_ASSERT(models::Arithmetic<unsigned short>);
CONCEPT_ASSERT(models::Arithmetic<unsigned short, unsigned short>);
CONCEPT_ASSERT(models::Arithmetic<unsigned short, unsigned short, unsigned short>);

CONCEPT_ASSERT(models::Arithmetic<int>);
CONCEPT_ASSERT(models::Arithmetic<int, int>);
CONCEPT_ASSERT(models::Arithmetic<int, int>);

CONCEPT_ASSERT(models::Arithmetic<unsigned int>);
CONCEPT_ASSERT(models::Arithmetic<unsigned int, unsigned int>);
CONCEPT_ASSERT(models::Arithmetic<unsigned int, unsigned int, unsigned int>);

CONCEPT_ASSERT(models::Arithmetic<long>);
CONCEPT_ASSERT(models::Arithmetic<long, long>);
CONCEPT_ASSERT(models::Arithmetic<long, long, long>);

CONCEPT_ASSERT(models::Arithmetic<unsigned long>);
CONCEPT_ASSERT(models::Arithmetic<unsigned long, unsigned long>);
CONCEPT_ASSERT(models::Arithmetic<unsigned long, unsigned long, unsigned long>);

CONCEPT_ASSERT(models::Arithmetic<long long>);
CONCEPT_ASSERT(models::Arithmetic<long long, long long>);
CONCEPT_ASSERT(models::Arithmetic<long long, long long, long long>);

CONCEPT_ASSERT(models::Arithmetic<unsigned long long>);
CONCEPT_ASSERT(models::Arithmetic<unsigned long long, unsigned long long>);
CONCEPT_ASSERT(models::Arithmetic<unsigned long long, unsigned long long, unsigned long long>);

CONCEPT_ASSERT(models::Arithmetic<float>);
CONCEPT_ASSERT(models::Arithmetic<float, float>);
CONCEPT_ASSERT(models::Arithmetic<float, float, float>);

CONCEPT_ASSERT(models::Arithmetic<double>);
CONCEPT_ASSERT(models::Arithmetic<double, double>);
CONCEPT_ASSERT(models::Arithmetic<double, double, double>);

CONCEPT_ASSERT(models::Arithmetic<long double>);
CONCEPT_ASSERT(models::Arithmetic<long double, long double>);
CONCEPT_ASSERT(models::Arithmetic<long double, long double, long double>);

CONCEPT_ASSERT(models::Arithmetic<wchar_t>);
CONCEPT_ASSERT(models::Arithmetic<wchar_t, wchar_t>);
CONCEPT_ASSERT(models::Arithmetic<wchar_t, wchar_t, wchar_t>);

CONCEPT_ASSERT(models::Arithmetic<char16_t>);
CONCEPT_ASSERT(models::Arithmetic<char16_t, char16_t>);
CONCEPT_ASSERT(models::Arithmetic<char16_t, char16_t, char16_t>);

CONCEPT_ASSERT(models::Arithmetic<char32_t>);
CONCEPT_ASSERT(models::Arithmetic<char32_t, char32_t>);
CONCEPT_ASSERT(models::Arithmetic<char32_t, char32_t, char32_t>);

CONCEPT_ASSERT(models::Arithmetic<bool, int>);
CONCEPT_ASSERT(models::Arithmetic<bool, int, double>);
CONCEPT_ASSERT(models::Arithmetic<int, bool>);
CONCEPT_ASSERT(models::Arithmetic<int, bool, double>);
CONCEPT_ASSERT(models::Arithmetic<double, bool>);
CONCEPT_ASSERT(models::Arithmetic<double, bool, int>);

CONCEPT_ASSERT(models::Arithmetic<short, int>);
CONCEPT_ASSERT(models::Arithmetic<short, long, int>);
CONCEPT_ASSERT(models::Arithmetic<short, long, int, double>);
CONCEPT_ASSERT(models::Arithmetic<int, short>);
CONCEPT_ASSERT(models::Arithmetic<int, short, long>);
CONCEPT_ASSERT(models::Arithmetic<int, double, short, long>);
CONCEPT_ASSERT(models::Arithmetic<long, short>);
CONCEPT_ASSERT(models::Arithmetic<long, int, short>);
CONCEPT_ASSERT(models::Arithmetic<long, int, double, short>);
CONCEPT_ASSERT(models::Arithmetic<double, int>);
CONCEPT_ASSERT(models::Arithmetic<double, long, int>);
CONCEPT_ASSERT(models::Arithmetic<double, long, int, short>);

CONCEPT_ASSERT(models::Arithmetic<float, double>);
CONCEPT_ASSERT(models::Arithmetic<float, double, short>);

CONCEPT_ASSERT(models::Arithmetic<double, float>);
CONCEPT_ASSERT(models::Arithmetic<double, float, int>);

CONCEPT_ASSERT(models::Arithmetic<int, double, wchar_t>);
CONCEPT_ASSERT(models::Arithmetic<double, int, wchar_t>);

CONCEPT_ASSERT(models::Arithmetic<wchar_t, double>);
CONCEPT_ASSERT(models::Arithmetic<wchar_t, int, double>);

// pointers and references
CONCEPT_ASSERT(not models::Arithmetic<long*>);
CONCEPT_ASSERT(not models::Arithmetic<long*, long*>);
CONCEPT_ASSERT(not models::Arithmetic<long*, long*, long*>);

CONCEPT_ASSERT(not models::Arithmetic<const char16_t*>);
CONCEPT_ASSERT(not models::Arithmetic<const char16_t*, const char16_t*>);
CONCEPT_ASSERT(not models::Arithmetic<const char16_t*, const char16_t*, const char16_t*>);

CONCEPT_ASSERT(not models::Arithmetic<long double*, const long double*>);
CONCEPT_ASSERT(not models::Arithmetic<const long double*, long double*>);
CONCEPT_ASSERT(not models::Arithmetic<const long double*, const long double*>);

CONCEPT_ASSERT(not models::Arithmetic<char*, char*, const char*>);
CONCEPT_ASSERT(not models::Arithmetic<char*, const char*, char*>);
CONCEPT_ASSERT(not models::Arithmetic<char*, const char*, const char*>);
CONCEPT_ASSERT(not models::Arithmetic<const char*, char*, char*>);
CONCEPT_ASSERT(not models::Arithmetic<const char*, char*, const char*>);
CONCEPT_ASSERT(not models::Arithmetic<const char*, const char*, char*>);
CONCEPT_ASSERT(not models::Arithmetic<const char*, const char*, const char*>);

CONCEPT_ASSERT(not models::Arithmetic<int&>);
CONCEPT_ASSERT(not models::Arithmetic<int&, int&>);
CONCEPT_ASSERT(not models::Arithmetic<int&, int&, int&>);

CONCEPT_ASSERT(not models::Arithmetic<const char32_t&>);
CONCEPT_ASSERT(not models::Arithmetic<const char32_t&, const char32_t&>);
CONCEPT_ASSERT(not models::Arithmetic<const char32_t&, const char32_t&, const char32_t&>);

CONCEPT_ASSERT(not models::Arithmetic<char32_t&, const char32_t&>);
CONCEPT_ASSERT(not models::Arithmetic<const char32_t&, char32_t&>);
CONCEPT_ASSERT(not models::Arithmetic<const char32_t&, const char32_t&>);

CONCEPT_ASSERT(not models::Arithmetic<short&, short&, const short&>);
CONCEPT_ASSERT(not models::Arithmetic<short&, const short&, short&>);
CONCEPT_ASSERT(not models::Arithmetic<short&, const short&, const short&>);
CONCEPT_ASSERT(not models::Arithmetic<const short&, short&, short&>);
CONCEPT_ASSERT(not models::Arithmetic<const short&, short&, const short&>);
CONCEPT_ASSERT(not models::Arithmetic<const short&, const short&, short&>);
CONCEPT_ASSERT(not models::Arithmetic<const short&, const short&, const short&>);

CONCEPT_ASSERT(not models::Arithmetic<int, long*>);
CONCEPT_ASSERT(not models::Arithmetic<int, long*, double>);
CONCEPT_ASSERT(not models::Arithmetic<int, double, long*>);

CONCEPT_ASSERT(not models::Arithmetic<int, const long*>);
CONCEPT_ASSERT(not models::Arithmetic<int, const long*, double>);
CONCEPT_ASSERT(not models::Arithmetic<int, double, const long*>);

CONCEPT_ASSERT(not models::Arithmetic<int*, float>);
CONCEPT_ASSERT(not models::Arithmetic<int*, float, wchar_t>);
CONCEPT_ASSERT(not models::Arithmetic<int*, wchar_t, float>);

CONCEPT_ASSERT(not models::Arithmetic<const int*, float>);
CONCEPT_ASSERT(not models::Arithmetic<const int*, float, wchar_t>);
CONCEPT_ASSERT(not models::Arithmetic<const int*, wchar_t, float>);

// standard compound types
CONCEPT_ASSERT(not models::Arithmetic<std::complex<double>>);
CONCEPT_ASSERT(not models::Arithmetic<std::vector<int>>);
CONCEPT_ASSERT(not models::Arithmetic<std::string>);

