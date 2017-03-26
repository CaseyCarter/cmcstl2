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
#include <stl2/detail/concepts/number.hpp>
#include "../detail/int128.hpp"
#include <complex> // TODO: replace with <stl2/detail/complex.hpp>
#include <string>
#include <vector>

namespace ranges = __stl2;
using cmcstl2::Uint128;

// fundamental types
CONCEPT_ASSERT(!ranges::Number<bool>());
CONCEPT_ASSERT(!ranges::OrderedNumber<bool>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<bool>());
CONCEPT_ASSERT(!ranges::Number<bool, bool>());
CONCEPT_ASSERT(!ranges::OrderedNumber<bool, bool>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<bool, bool>());
CONCEPT_ASSERT(!ranges::Number<bool, bool, bool>());
CONCEPT_ASSERT(!ranges::OrderedNumber<bool, bool, bool>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<bool, bool, bool>());

CONCEPT_ASSERT(!ranges::Number<char>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char, char>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char>());
CONCEPT_ASSERT(!ranges::Number<char>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char, char>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char, char>());
CONCEPT_ASSERT(!ranges::Number<char, char, char>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char, char, char>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char, char, char>());

CONCEPT_ASSERT(ranges::Number<signed char>());
CONCEPT_ASSERT(ranges::OrderedNumber<signed char>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<signed char>());
CONCEPT_ASSERT(ranges::Number<signed char, signed char>());
CONCEPT_ASSERT(ranges::OrderedNumber<signed char, signed char>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<signed char, signed char>());
CONCEPT_ASSERT(ranges::Number<signed char, signed char, signed char>());
CONCEPT_ASSERT(ranges::OrderedNumber<signed char, signed char, signed char>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<signed char, signed char, signed char>());

CONCEPT_ASSERT(ranges::Number<unsigned char>());
CONCEPT_ASSERT(ranges::OrderedNumber<unsigned char>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<unsigned char>());
CONCEPT_ASSERT(ranges::Number<unsigned char, unsigned char>());
CONCEPT_ASSERT(ranges::OrderedNumber<unsigned char, unsigned char>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<unsigned char, unsigned char>());
CONCEPT_ASSERT(ranges::Number<unsigned char, unsigned char, unsigned char>());
CONCEPT_ASSERT(ranges::OrderedNumber<unsigned char, unsigned char, unsigned char>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<unsigned char, unsigned char, unsigned char>());

CONCEPT_ASSERT(ranges::Number<short>());
CONCEPT_ASSERT(ranges::OrderedNumber<short>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<short>());
CONCEPT_ASSERT(ranges::Number<short, short>());
CONCEPT_ASSERT(ranges::OrderedNumber<short, short>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<short, short>());
CONCEPT_ASSERT(ranges::Number<short, short, short>());
CONCEPT_ASSERT(ranges::OrderedNumber<short, short, short>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<short, short, short>());

CONCEPT_ASSERT(ranges::Number<unsigned short>());
CONCEPT_ASSERT(ranges::OrderedNumber<unsigned short>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<unsigned short>());
CONCEPT_ASSERT(ranges::Number<unsigned short, unsigned short>());
CONCEPT_ASSERT(ranges::OrderedNumber<unsigned short, unsigned short>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<unsigned short, unsigned short>());
CONCEPT_ASSERT(ranges::Number<unsigned short, unsigned short, unsigned short>());
CONCEPT_ASSERT(ranges::OrderedNumber<unsigned short, unsigned short, unsigned short>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<unsigned short, unsigned short, unsigned short>());

CONCEPT_ASSERT(ranges::Number<int>());
CONCEPT_ASSERT(ranges::OrderedNumber<int>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<int>());
CONCEPT_ASSERT(ranges::Number<int, int>());
CONCEPT_ASSERT(ranges::OrderedNumber<int, int>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<int, int>());
CONCEPT_ASSERT(ranges::Number<int, int, int>());
CONCEPT_ASSERT(ranges::OrderedNumber<int, int, int>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<int, int, int>());

CONCEPT_ASSERT(ranges::Number<unsigned int>());
CONCEPT_ASSERT(ranges::OrderedNumber<unsigned int>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<unsigned int>());
CONCEPT_ASSERT(ranges::Number<unsigned int, unsigned int>());
CONCEPT_ASSERT(ranges::OrderedNumber<unsigned int, unsigned int>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<unsigned int, unsigned int>());
CONCEPT_ASSERT(ranges::Number<unsigned int, unsigned int, unsigned int>());
CONCEPT_ASSERT(ranges::OrderedNumber<unsigned int, unsigned int, unsigned int>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<unsigned int, unsigned int, unsigned int>());

CONCEPT_ASSERT(ranges::Number<long>());
CONCEPT_ASSERT(ranges::OrderedNumber<long>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<long>());
CONCEPT_ASSERT(ranges::Number<long, long>());
CONCEPT_ASSERT(ranges::OrderedNumber<long, long>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<long, long>());
CONCEPT_ASSERT(ranges::Number<long, long, long>());
CONCEPT_ASSERT(ranges::OrderedNumber<long, long, long>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<long, long, long>());

CONCEPT_ASSERT(ranges::Number<unsigned long>());
CONCEPT_ASSERT(ranges::OrderedNumber<unsigned long>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<unsigned long>());
CONCEPT_ASSERT(ranges::Number<unsigned long, unsigned long>());
CONCEPT_ASSERT(ranges::OrderedNumber<unsigned long, unsigned long>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<unsigned long, unsigned long>());
CONCEPT_ASSERT(ranges::Number<unsigned long, unsigned long, unsigned long>());
CONCEPT_ASSERT(ranges::OrderedNumber<unsigned long, unsigned long, unsigned long>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<unsigned long, unsigned long, unsigned long>());

CONCEPT_ASSERT(ranges::Number<long long>());
CONCEPT_ASSERT(ranges::OrderedNumber<long long>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<long long>());
CONCEPT_ASSERT(ranges::Number<long long, long long>());
CONCEPT_ASSERT(ranges::OrderedNumber<long long, long long>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<long long, long long>());
CONCEPT_ASSERT(ranges::Number<long long, long long, long long>());
CONCEPT_ASSERT(ranges::OrderedNumber<long long, long long, long long>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<long long, long long, long long>());

CONCEPT_ASSERT(ranges::Number<unsigned long long>());
CONCEPT_ASSERT(ranges::OrderedNumber<unsigned long long>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<unsigned long long>());
CONCEPT_ASSERT(ranges::Number<unsigned long long, unsigned long long>());
CONCEPT_ASSERT(ranges::OrderedNumber<unsigned long long, unsigned long long>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<unsigned long long, unsigned long long>());
CONCEPT_ASSERT(ranges::Number<unsigned long long, unsigned long long, unsigned long long>());
CONCEPT_ASSERT(ranges::OrderedNumber<unsigned long long, unsigned long long, unsigned long long>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<unsigned long long, unsigned long long, unsigned long long>());

CONCEPT_ASSERT(ranges::Number<float>());
CONCEPT_ASSERT(ranges::OrderedNumber<float>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<float>());
CONCEPT_ASSERT(ranges::Number<float, float>());
CONCEPT_ASSERT(ranges::OrderedNumber<float, float>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<float, float>());
CONCEPT_ASSERT(ranges::Number<float, float, float>());
CONCEPT_ASSERT(ranges::OrderedNumber<float, float, float>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<float, float, float>());

CONCEPT_ASSERT(ranges::Number<double>());
CONCEPT_ASSERT(ranges::OrderedNumber<double>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<double>());
CONCEPT_ASSERT(ranges::Number<double, double>());
CONCEPT_ASSERT(ranges::OrderedNumber<double, double>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<double, double>());
CONCEPT_ASSERT(ranges::Number<double, double, double>());
CONCEPT_ASSERT(ranges::OrderedNumber<double, double, double>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<double, double, double>());

CONCEPT_ASSERT(ranges::Number<long double>());
CONCEPT_ASSERT(ranges::OrderedNumber<long double>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<long double>());
CONCEPT_ASSERT(ranges::Number<long double, long double>());
CONCEPT_ASSERT(ranges::OrderedNumber<long double, long double>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<long double, long double>());
CONCEPT_ASSERT(ranges::Number<long double, long double, long double>());
CONCEPT_ASSERT(ranges::OrderedNumber<long double, long double, long double>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<long double, long double, long double>());

CONCEPT_ASSERT(!ranges::Number<wchar_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<wchar_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<wchar_t>());
CONCEPT_ASSERT(!ranges::Number<wchar_t, wchar_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<wchar_t, wchar_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<wchar_t, wchar_t>());
CONCEPT_ASSERT(!ranges::Number<wchar_t, wchar_t, wchar_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<wchar_t, wchar_t, wchar_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<wchar_t, wchar_t, wchar_t>());

CONCEPT_ASSERT(!ranges::Number<char16_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char16_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char16_t>());
CONCEPT_ASSERT(!ranges::Number<char16_t, char16_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char16_t, char16_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char16_t, char16_t>());
CONCEPT_ASSERT(!ranges::Number<char16_t, char16_t, char16_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char16_t, char16_t, char16_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char16_t, char16_t, char16_t>());

CONCEPT_ASSERT(!ranges::Number<char32_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char32_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char32_t>());
CONCEPT_ASSERT(!ranges::Number<char32_t, char32_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char32_t, char32_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char32_t, char32_t>());
CONCEPT_ASSERT(!ranges::Number<char32_t, char32_t, char32_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char32_t, char32_t, char32_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char32_t, char32_t, char32_t>());

// bools are lepers, make sure that there are no associates...
CONCEPT_ASSERT(!ranges::Number<bool, int>());
CONCEPT_ASSERT(!ranges::OrderedNumber<bool, int>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<bool, int>());
CONCEPT_ASSERT(!ranges::Number<bool, int, double>());
CONCEPT_ASSERT(!ranges::OrderedNumber<bool, int, double>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<bool, int, double>());
CONCEPT_ASSERT(!ranges::Number<int, bool>());
CONCEPT_ASSERT(!ranges::OrderedNumber<int, bool>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<int, bool>());
CONCEPT_ASSERT(!ranges::Number<int, bool, double>());
CONCEPT_ASSERT(!ranges::OrderedNumber<int, bool, double>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<int, bool, double>());
CONCEPT_ASSERT(!ranges::Number<double, bool>());
CONCEPT_ASSERT(!ranges::OrderedNumber<double, bool>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<double, bool>());
CONCEPT_ASSERT(!ranges::Number<double, bool, int>());
CONCEPT_ASSERT(!ranges::OrderedNumber<double, bool, int>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<double, bool, int>());
CONCEPT_ASSERT(!ranges::Number<bool, char>());
CONCEPT_ASSERT(!ranges::OrderedNumber<bool, char>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<bool, char>());
CONCEPT_ASSERT(!ranges::Number<bool, wchar_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<bool, wchar_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<bool, wchar_t>());
CONCEPT_ASSERT(!ranges::Number<bool, char16_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<bool, char16_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<bool, char16_t>());
CONCEPT_ASSERT(!ranges::Number<bool, char32_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<bool, char32_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<bool, char32_t>());

// char needs to be lonely
CONCEPT_ASSERT(!ranges::Number<char, long>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char, long>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char, long>());
CONCEPT_ASSERT(!ranges::Number<char, long, double>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char, long, double>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char, long, double>());
CONCEPT_ASSERT(!ranges::Number<long, char>());
CONCEPT_ASSERT(!ranges::OrderedNumber<long, char>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<long, char>());
CONCEPT_ASSERT(!ranges::Number<long, char, double>());
CONCEPT_ASSERT(!ranges::OrderedNumber<long, char, double>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<long, char, double>());
CONCEPT_ASSERT(!ranges::Number<double, char>());
CONCEPT_ASSERT(!ranges::OrderedNumber<double, char>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<double, char>());
CONCEPT_ASSERT(!ranges::Number<double, char, long>());
CONCEPT_ASSERT(!ranges::OrderedNumber<double, char, long>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<double, char, long>());
CONCEPT_ASSERT(!ranges::Number<char, bool>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char, bool>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char, bool>());
CONCEPT_ASSERT(!ranges::Number<char, wchar_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char, wchar_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char, wchar_t>());
CONCEPT_ASSERT(!ranges::Number<char, char16_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char, char16_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char, char16_t>());
CONCEPT_ASSERT(!ranges::Number<char, char32_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char, char32_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char, char32_t>());

// wchar_t is also a leper, make sure there are no associates...
CONCEPT_ASSERT(!ranges::Number<wchar_t, long long>());
CONCEPT_ASSERT(!ranges::OrderedNumber<wchar_t, long long>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<wchar_t, long long>());
CONCEPT_ASSERT(!ranges::Number<wchar_t, long long, float>());
CONCEPT_ASSERT(!ranges::OrderedNumber<wchar_t, long long, float>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<wchar_t, long long, float>());
CONCEPT_ASSERT(!ranges::Number<long long, wchar_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<long long, wchar_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<long long, wchar_t>());
CONCEPT_ASSERT(!ranges::Number<long long, wchar_t, float>());
CONCEPT_ASSERT(!ranges::OrderedNumber<long long, wchar_t, float>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<long long, wchar_t, float>());
CONCEPT_ASSERT(!ranges::Number<float, wchar_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<float, wchar_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<float, wchar_t>());
CONCEPT_ASSERT(!ranges::Number<float, wchar_t, long long>());
CONCEPT_ASSERT(!ranges::OrderedNumber<float, wchar_t, long long>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<float, wchar_t, long long>());
CONCEPT_ASSERT(!ranges::Number<wchar_t, bool>());
CONCEPT_ASSERT(!ranges::OrderedNumber<wchar_t, bool>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<wchar_t, bool>());
CONCEPT_ASSERT(!ranges::Number<wchar_t, char>());
CONCEPT_ASSERT(!ranges::OrderedNumber<wchar_t, char>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<wchar_t, char>());
CONCEPT_ASSERT(!ranges::Number<wchar_t, char16_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<wchar_t, char16_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<wchar_t, char16_t>());
CONCEPT_ASSERT(!ranges::Number<wchar_t, char32_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<wchar_t, char32_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<wchar_t, char32_t>());

// char16_t can't be associated with anyone either...
CONCEPT_ASSERT(!ranges::Number<char16_t, short>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char16_t, short>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char16_t, short>());
CONCEPT_ASSERT(!ranges::Number<char16_t, short, long double>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char16_t, short, long double>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char16_t, short, long double>());
CONCEPT_ASSERT(!ranges::Number<short, char16_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<short, char16_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<short, char16_t>());
CONCEPT_ASSERT(!ranges::Number<short, char16_t, long double>());
CONCEPT_ASSERT(!ranges::OrderedNumber<short, char16_t, long double>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<short, char16_t, long double>());
CONCEPT_ASSERT(!ranges::Number<long double, char16_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<long double, char16_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<long double, char16_t>());
CONCEPT_ASSERT(!ranges::Number<long double, char16_t, short>());
CONCEPT_ASSERT(!ranges::OrderedNumber<long double, char16_t, short>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<long double, char16_t, short>());
CONCEPT_ASSERT(!ranges::Number<char16_t, bool>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char16_t, bool>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char16_t, bool>());
CONCEPT_ASSERT(!ranges::Number<char16_t, char>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char16_t, char>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char16_t, char>());
CONCEPT_ASSERT(!ranges::Number<char16_t, wchar_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char16_t, wchar_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char16_t, wchar_t>());
CONCEPT_ASSERT(!ranges::Number<char16_t, char32_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char16_t, char32_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char16_t, char32_t>());

// char32_t is the last fundamental that isn't allowed friends
CONCEPT_ASSERT(!ranges::Number<char32_t, short>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char32_t, short>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char32_t, short>());
CONCEPT_ASSERT(!ranges::Number<char32_t, short, long double>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char32_t, short, long double>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char32_t, short, long double>());
CONCEPT_ASSERT(!ranges::Number<short, char32_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<short, char32_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<short, char32_t>());
CONCEPT_ASSERT(!ranges::Number<short, char32_t, long double>());
CONCEPT_ASSERT(!ranges::OrderedNumber<short, char32_t, long double>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<short, char32_t, long double>());
CONCEPT_ASSERT(!ranges::Number<long double, char32_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<long double, char32_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<long double, char32_t>());
CONCEPT_ASSERT(!ranges::Number<long double, char32_t, short>());
CONCEPT_ASSERT(!ranges::OrderedNumber<long double, char32_t, short>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<long double, char32_t, short>());
CONCEPT_ASSERT(!ranges::Number<char32_t, bool>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char32_t, bool>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char32_t, bool>());
CONCEPT_ASSERT(!ranges::Number<char32_t, char>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char32_t, char>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char32_t, char>());
CONCEPT_ASSERT(!ranges::Number<char32_t, wchar_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char32_t, wchar_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char32_t, wchar_t>());
CONCEPT_ASSERT(!ranges::Number<char32_t, char16_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char32_t, char16_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char32_t, char16_t>());

CONCEPT_ASSERT(ranges::Number<short, int>());
CONCEPT_ASSERT(ranges::OrderedNumber<short, int>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<short, int>());
CONCEPT_ASSERT(ranges::Number<short, long, int>());
CONCEPT_ASSERT(ranges::OrderedNumber<short, long, int>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<short, long, int>());
CONCEPT_ASSERT(ranges::Number<short, long, int, double>());
CONCEPT_ASSERT(ranges::OrderedNumber<short, long, int, double>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<short, long, int, double>());
CONCEPT_ASSERT(ranges::Number<int, short>());
CONCEPT_ASSERT(ranges::OrderedNumber<int, short>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<int, short>());
CONCEPT_ASSERT(ranges::Number<int, short, long>());
CONCEPT_ASSERT(ranges::OrderedNumber<int, short, long>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<int, short, long>());
CONCEPT_ASSERT(ranges::Number<int, double, short, long>());
CONCEPT_ASSERT(ranges::OrderedNumber<int, double, short, long>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<int, double, short, long>());
CONCEPT_ASSERT(ranges::Number<long, short>());
CONCEPT_ASSERT(ranges::OrderedNumber<long, short>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<long, short>());
CONCEPT_ASSERT(ranges::Number<long, int, short>());
CONCEPT_ASSERT(ranges::OrderedNumber<long, int, short>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<long, int, short>());
CONCEPT_ASSERT(ranges::Number<long, int, double, short>());
CONCEPT_ASSERT(ranges::OrderedNumber<long, int, double, short>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<long, int, double, short>());
CONCEPT_ASSERT(ranges::Number<double, int>());
CONCEPT_ASSERT(ranges::OrderedNumber<double, int>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<double, int>());
CONCEPT_ASSERT(ranges::Number<double, long, int>());
CONCEPT_ASSERT(ranges::OrderedNumber<double, long, int>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<double, long, int>());
CONCEPT_ASSERT(ranges::Number<double, long, int, short>());
CONCEPT_ASSERT(ranges::OrderedNumber<double, long, int, short>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<double, long, int, short>());

CONCEPT_ASSERT(ranges::Number<float, double>());
CONCEPT_ASSERT(ranges::OrderedNumber<float, double>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<float, double>());
CONCEPT_ASSERT(ranges::Number<float, double, short>());
CONCEPT_ASSERT(ranges::OrderedNumber<float, double, short>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<float, double, short>());

CONCEPT_ASSERT(ranges::Number<double, float>());
CONCEPT_ASSERT(ranges::OrderedNumber<double, float>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<double, float>());
CONCEPT_ASSERT(ranges::Number<double, float, int>());
CONCEPT_ASSERT(ranges::OrderedNumber<double, float, int>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<double, float, int>());

// pointers and references
CONCEPT_ASSERT(!ranges::Number<long*>());
CONCEPT_ASSERT(!ranges::OrderedNumber<long*>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<long*>());
CONCEPT_ASSERT(!ranges::Number<long*, long*>());
CONCEPT_ASSERT(!ranges::OrderedNumber<long*, long*>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<long*, long*>());
CONCEPT_ASSERT(!ranges::Number<long*, long*, long*>());
CONCEPT_ASSERT(!ranges::OrderedNumber<long*, long*, long*>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<long*, long*, long*>());

CONCEPT_ASSERT(!ranges::Number<const char16_t*>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const char16_t*>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const char16_t*>());
CONCEPT_ASSERT(!ranges::Number<const char16_t*, const char16_t*>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const char16_t*, const char16_t*>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const char16_t*, const char16_t*>());
CONCEPT_ASSERT(!ranges::Number<const char16_t*, const char16_t*, const char16_t*>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const char16_t*, const char16_t*, const char16_t*>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const char16_t*, const char16_t*, const char16_t*>());

CONCEPT_ASSERT(!ranges::Number<long double*, const long double*>());
CONCEPT_ASSERT(!ranges::OrderedNumber<long double*, const long double*>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<long double*, const long double*>());
CONCEPT_ASSERT(!ranges::Number<const long double*, long double*>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const long double*, long double*>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const long double*, long double*>());
CONCEPT_ASSERT(!ranges::Number<const long double*, const long double*>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const long double*, const long double*>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const long double*, const long double*>());
CONCEPT_ASSERT(!ranges::Number<char*, char*, const char*>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char*, char*, const char*>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char*, char*, const char*>());
CONCEPT_ASSERT(!ranges::Number<char*, const char*, char*>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char*, const char*, char*>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char*, const char*, char*>());
CONCEPT_ASSERT(!ranges::Number<char*, const char*, const char*>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char*, const char*, const char*>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char*, const char*, const char*>());
CONCEPT_ASSERT(!ranges::Number<const char*, char*, char*>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const char*, char*, char*>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const char*, char*, char*>());
CONCEPT_ASSERT(!ranges::Number<const char*, char*, const char*>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const char*, char*, const char*>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const char*, char*, const char*>());
CONCEPT_ASSERT(!ranges::Number<const char*, const char*, char*>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const char*, const char*, char*>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const char*, const char*, char*>());
CONCEPT_ASSERT(!ranges::Number<const char*, const char*, const char*>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const char*, const char*, const char*>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const char*, const char*, const char*>());

CONCEPT_ASSERT(!ranges::Number<int&>());
CONCEPT_ASSERT(!ranges::OrderedNumber<int&>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<int&>());
CONCEPT_ASSERT(!ranges::Number<int&, int&>());
CONCEPT_ASSERT(!ranges::OrderedNumber<int&, int&>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<int&, int&>());
CONCEPT_ASSERT(!ranges::Number<int&, int&, int&>());
CONCEPT_ASSERT(!ranges::OrderedNumber<int&, int&, int&>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<int&, int&, int&>());

CONCEPT_ASSERT(!ranges::Number<const char32_t&>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const char32_t&>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const char32_t&>());
CONCEPT_ASSERT(!ranges::Number<const char32_t&, const char32_t&>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const char32_t&, const char32_t&>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const char32_t&, const char32_t&>());
CONCEPT_ASSERT(!ranges::Number<const char32_t&, const char32_t&, const char32_t&>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const char32_t&, const char32_t&, const char32_t&>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const char32_t&, const char32_t&, const char32_t&>());

CONCEPT_ASSERT(!ranges::Number<char32_t&, const char32_t&>());
CONCEPT_ASSERT(!ranges::OrderedNumber<char32_t&, const char32_t&>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<char32_t&, const char32_t&>());
CONCEPT_ASSERT(!ranges::Number<const char32_t&, char32_t&>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const char32_t&, char32_t&>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const char32_t&, char32_t&>());
CONCEPT_ASSERT(!ranges::Number<const char32_t&, const char32_t&>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const char32_t&, const char32_t&>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const char32_t&, const char32_t&>());

CONCEPT_ASSERT(!ranges::Number<short&, short&, const short&>());
CONCEPT_ASSERT(!ranges::OrderedNumber<short&, short&, const short&>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<short&, short&, const short&>());
CONCEPT_ASSERT(!ranges::Number<short&, const short&, short&>());
CONCEPT_ASSERT(!ranges::OrderedNumber<short&, const short&, short&>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<short&, const short&, short&>());
CONCEPT_ASSERT(!ranges::Number<short&, const short&, const short&>());
CONCEPT_ASSERT(!ranges::OrderedNumber<short&, const short&, const short&>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<short&, const short&, const short&>());
CONCEPT_ASSERT(!ranges::Number<const short&, short&, short&>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const short&, short&, short&>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const short&, short&, short&>());
CONCEPT_ASSERT(!ranges::Number<const short&, short&, const short&>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const short&, short&, const short&>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const short&, short&, const short&>());
CONCEPT_ASSERT(!ranges::Number<const short&, const short&, short&>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const short&, const short&, short&>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const short&, const short&, short&>());
CONCEPT_ASSERT(!ranges::Number<const short&, const short&, const short&>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const short&, const short&, const short&>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const short&, const short&, const short&>());

CONCEPT_ASSERT(!ranges::Number<int&&>());
CONCEPT_ASSERT(!ranges::OrderedNumber<int&&>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<int&&>());
CONCEPT_ASSERT(!ranges::Number<int&&, int&&>());
CONCEPT_ASSERT(!ranges::OrderedNumber<int&&, int&&>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<int&&, int&&>());
CONCEPT_ASSERT(!ranges::Number<int&&, int&&, int&&>());
CONCEPT_ASSERT(!ranges::OrderedNumber<int&&, int&&, int&&>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<int&&, int&&, int&&>());

CONCEPT_ASSERT(!ranges::Number<int, long*>());
CONCEPT_ASSERT(!ranges::OrderedNumber<int, long*>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<int, long*>());
CONCEPT_ASSERT(!ranges::Number<int, long*, double>());
CONCEPT_ASSERT(!ranges::OrderedNumber<int, long*, double>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<int, long*, double>());
CONCEPT_ASSERT(!ranges::Number<int, double, long*>());
CONCEPT_ASSERT(!ranges::OrderedNumber<int, double, long*>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<int, double, long*>());

CONCEPT_ASSERT(!ranges::Number<int, const long*>());
CONCEPT_ASSERT(!ranges::OrderedNumber<int, const long*>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<int, const long*>());
CONCEPT_ASSERT(!ranges::Number<int, const long*, double>());
CONCEPT_ASSERT(!ranges::OrderedNumber<int, const long*, double>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<int, const long*, double>());
CONCEPT_ASSERT(!ranges::Number<int, double, const long*>());
CONCEPT_ASSERT(!ranges::OrderedNumber<int, double, const long*>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<int, double, const long*>());

CONCEPT_ASSERT(!ranges::Number<int*, float>());
CONCEPT_ASSERT(!ranges::OrderedNumber<int*, float>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<int*, float>());
CONCEPT_ASSERT(!ranges::Number<int*, float, wchar_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<int*, float, wchar_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<int*, float, wchar_t>());
CONCEPT_ASSERT(!ranges::Number<int*, wchar_t, float>());
CONCEPT_ASSERT(!ranges::OrderedNumber<int*, wchar_t, float>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<int*, wchar_t, float>());

CONCEPT_ASSERT(!ranges::Number<const int*, float>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const int*, float>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const int*, float>());
CONCEPT_ASSERT(!ranges::Number<const int*, float, wchar_t>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const int*, float, wchar_t>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const int*, float, wchar_t>());
CONCEPT_ASSERT(!ranges::Number<const int*, wchar_t, float>());
CONCEPT_ASSERT(!ranges::OrderedNumber<const int*, wchar_t, float>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<const int*, wchar_t, float>());

// complex is the reason Number exists
// we should thoroughly test that it is weak, and anything it combines with is weak at best
CONCEPT_ASSERT(ranges::Number<std::complex<double>>());
CONCEPT_ASSERT(!ranges::OrderedNumber<std::complex<double>>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<std::complex<double>>());
CONCEPT_ASSERT(ranges::Number<std::complex<double>, std::complex<double>>());
CONCEPT_ASSERT(!ranges::OrderedNumber<std::complex<double>, std::complex<double>>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<std::complex<double>, std::complex<double>>());
CONCEPT_ASSERT(ranges::Number<std::complex<double>, std::complex<double>, std::complex<double>>());
CONCEPT_ASSERT(!ranges::OrderedNumber<std::complex<double>, std::complex<double>, std::complex<double>>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<std::complex<double>, std::complex<double>, std::complex<double>>());

// test something that's genuinely not related to arithmetic types at all
CONCEPT_ASSERT(!ranges::Number<std::vector<int>>());
CONCEPT_ASSERT(!ranges::OrderedNumber<std::vector<int>>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<std::vector<int>>());

// custom Number compound type
namespace {
   class Fixed_point {
   public:
      Fixed_point() = default;
      Fixed_point(bool);
      Fixed_point& operator+=(const Fixed_point&) noexcept;
      Fixed_point& operator-=(const Fixed_point&) noexcept;
      Fixed_point& operator*=(const Fixed_point&) noexcept;
      Fixed_point& operator/=(const Fixed_point&) noexcept;

      Fixed_point operator+() const noexcept;
      Fixed_point operator-() const noexcept;

      Fixed_point operator+(Fixed_point) const noexcept;
      Fixed_point operator-(Fixed_point) const noexcept;
      Fixed_point operator*(Fixed_point) const noexcept;
      Fixed_point operator/(Fixed_point) const noexcept;

      bool operator==(const Fixed_point&) const noexcept;
      bool operator!=(const Fixed_point&) const noexcept;
   private:
      std::vector<double> d;
   };
}

CONCEPT_ASSERT(ranges::Number<Fixed_point>());
CONCEPT_ASSERT(!ranges::OrderedNumber<Fixed_point>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<Fixed_point>());
CONCEPT_ASSERT(ranges::Number<Fixed_point, Fixed_point>());
CONCEPT_ASSERT(!ranges::OrderedNumber<Fixed_point, Fixed_point>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<Fixed_point, Fixed_point>());
CONCEPT_ASSERT(ranges::Number<Fixed_point, Fixed_point, Fixed_point>());
CONCEPT_ASSERT(!ranges::OrderedNumber<Fixed_point, Fixed_point, Fixed_point>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<Fixed_point, Fixed_point, Fixed_point>());
CONCEPT_ASSERT(!ranges::Number<Fixed_point, int>());
CONCEPT_ASSERT(!ranges::OrderedNumber<Fixed_point, int>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<Fixed_point, int>());
CONCEPT_ASSERT(!ranges::Number<Fixed_point, int, double>());
CONCEPT_ASSERT(!ranges::OrderedNumber<Fixed_point, int, double>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<Fixed_point, int, double>());
CONCEPT_ASSERT(!ranges::Number<int, Fixed_point>());
CONCEPT_ASSERT(!ranges::OrderedNumber<int, Fixed_point>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<int, Fixed_point>());
CONCEPT_ASSERT(!ranges::Number<int, Fixed_point, double>());
CONCEPT_ASSERT(!ranges::OrderedNumber<int, Fixed_point, double>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<int, Fixed_point, double>());
CONCEPT_ASSERT(!ranges::Number<Fixed_point, std::complex<double>>());
CONCEPT_ASSERT(!ranges::OrderedNumber<Fixed_point, std::complex<double>>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<Fixed_point, std::complex<double>>());

CONCEPT_ASSERT(ranges::Number<Uint128>());
CONCEPT_ASSERT(ranges::RegularNumber<Uint128>());
CONCEPT_ASSERT(ranges::OrderedNumber<Uint128>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<Uint128>());
CONCEPT_ASSERT(ranges::Number<Uint128, Uint128>());
CONCEPT_ASSERT(ranges::RegularNumber<Uint128, Uint128>());
CONCEPT_ASSERT(ranges::OrderedNumber<Uint128, Uint128>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<Uint128, Uint128>());
CONCEPT_ASSERT(ranges::Number<Uint128, Uint128, Uint128>());
CONCEPT_ASSERT(ranges::RegularNumber<Uint128, Uint128, Uint128>());
CONCEPT_ASSERT(ranges::OrderedNumber<Uint128, Uint128, Uint128>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<Uint128, Uint128, Uint128>());

CONCEPT_ASSERT(ranges::Number<Uint128, std::int64_t>());
CONCEPT_ASSERT(ranges::RegularNumber<Uint128, std::int64_t>());
CONCEPT_ASSERT(ranges::OrderedNumber<Uint128, std::int64_t>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<Uint128, std::int64_t>());
CONCEPT_ASSERT(!ranges::Number<std::int64_t, Uint128>());
CONCEPT_ASSERT(!ranges::RegularNumber<std::int64_t, Uint128>());
CONCEPT_ASSERT(!ranges::OrderedNumber<std::int64_t, Uint128>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<std::int64_t, Uint128>());

CONCEPT_ASSERT(ranges::Number<Uint128, std::uint64_t>());
CONCEPT_ASSERT(ranges::RegularNumber<Uint128, std::uint64_t>());
CONCEPT_ASSERT(ranges::OrderedNumber<Uint128, std::uint64_t>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<Uint128, std::uint64_t>());
CONCEPT_ASSERT(!ranges::Number<std::uint64_t, Uint128>());
CONCEPT_ASSERT(!ranges::RegularNumber<std::uint64_t, Uint128>());
CONCEPT_ASSERT(!ranges::OrderedNumber<std::uint64_t, Uint128>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<std::uint64_t, Uint128>());

CONCEPT_ASSERT(ranges::Number<Uint128, double>());
CONCEPT_ASSERT(ranges::RegularNumber<Uint128, double>());
CONCEPT_ASSERT(ranges::OrderedNumber<Uint128, double>());
CONCEPT_ASSERT(ranges::RegularOrderedNumber<Uint128, double>());
CONCEPT_ASSERT(!ranges::Number<double, Uint128>());
CONCEPT_ASSERT(!ranges::RegularNumber<double, Uint128>());
CONCEPT_ASSERT(!ranges::OrderedNumber<double, Uint128>());
CONCEPT_ASSERT(!ranges::RegularOrderedNumber<double, Uint128>());

CONCEPT_ASSERT(ranges::BitwiseNumber<signed char, unsigned char>());
CONCEPT_ASSERT(ranges::BitwiseNumber<unsigned char, unsigned char>());
CONCEPT_ASSERT(ranges::BitwiseNumber<short, unsigned char>());
CONCEPT_ASSERT(ranges::BitwiseNumber<short, unsigned short>());
CONCEPT_ASSERT(ranges::BitwiseNumber<Uint128, unsigned int>());

CONCEPT_ASSERT(!ranges::BitwiseNumber<signed char, char>());
CONCEPT_ASSERT(!ranges::BitwiseNumber<signed char, signed char>());
CONCEPT_ASSERT(!ranges::BitwiseNumber<signed char, short>());
CONCEPT_ASSERT(!ranges::BitwiseNumber<signed char, unsigned short>());
CONCEPT_ASSERT(!ranges::BitwiseNumber<double, unsigned int>());
CONCEPT_ASSERT(!ranges::BitwiseNumber<Fixed_point, unsigned int>());

int main()
{
}
