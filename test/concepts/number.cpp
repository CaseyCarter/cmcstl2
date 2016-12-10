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
CONCEPT_ASSERT(!models::Number<bool> &&
               !models::StrictNumber<bool> &&
               !models::RegularStrictNumber<bool>);
CONCEPT_ASSERT(!models::Number<bool, bool> &&
               !models::StrictNumber<bool, bool> &&
               !models::RegularStrictNumber<bool, bool>);
CONCEPT_ASSERT(!models::Number<bool, bool, bool> &&
               !models::StrictNumber<bool, bool, bool> &&
               !models::RegularStrictNumber<bool, bool, bool>);

CONCEPT_ASSERT(!models::Number<char> &&
               !models::StrictNumber<char, char> &&
               !models::RegularStrictNumber<char>);
CONCEPT_ASSERT(!models::Number<char> &&
               !models::StrictNumber<char, char> &&
               !models::RegularStrictNumber<char, char>);
CONCEPT_ASSERT(!models::Number<char, char, char> &&
               !models::StrictNumber<char, char, char> &&
               !models::RegularStrictNumber<char, char, char>);

CONCEPT_ASSERT(models::Number<signed char> &&
               models::StrictNumber<signed char> &&
               models::RegularStrictNumber<signed char>);
CONCEPT_ASSERT(models::Number<signed char, signed char> &&
               models::StrictNumber<signed char, signed char> &&
               models::RegularStrictNumber<signed char, signed char>);
CONCEPT_ASSERT(models::Number<signed char, signed char, signed char> &&
               models::StrictNumber<signed char, signed char, signed char> &&
               models::RegularStrictNumber<signed char, signed char, signed char>);

CONCEPT_ASSERT(models::Number<unsigned char> &&
               models::StrictNumber<unsigned char> &&
               models::RegularStrictNumber<unsigned char>);
CONCEPT_ASSERT(models::Number<unsigned char, unsigned char> &&
               models::StrictNumber<unsigned char, unsigned char> &&
               models::RegularStrictNumber<unsigned char, unsigned char>);
CONCEPT_ASSERT(models::Number<unsigned char, unsigned char, unsigned char> &&
               models::StrictNumber<unsigned char, unsigned char, unsigned char> &&
               models::RegularStrictNumber<unsigned char, unsigned char, unsigned char>);

CONCEPT_ASSERT(models::Number<short> &&
               models::StrictNumber<short> &&
               models::RegularStrictNumber<short>);
CONCEPT_ASSERT(models::Number<short, short> &&
               models::StrictNumber<short, short> &&
               models::RegularStrictNumber<short, short>);
CONCEPT_ASSERT(models::Number<short, short, short> &&
               models::StrictNumber<short, short, short> &&
               models::RegularStrictNumber<short, short, short>);

CONCEPT_ASSERT(models::Number<unsigned short> &&
               models::StrictNumber<unsigned short> &&
               models::RegularStrictNumber<unsigned short>);
CONCEPT_ASSERT(models::Number<unsigned short, unsigned short> &&
               models::StrictNumber<unsigned short, unsigned short> &&
               models::RegularStrictNumber<unsigned short, unsigned short>);
CONCEPT_ASSERT(models::Number<unsigned short, unsigned short, unsigned short> &&
               models::StrictNumber<unsigned short, unsigned short, unsigned short> &&
               models::RegularStrictNumber<unsigned short, unsigned short, unsigned short>);

CONCEPT_ASSERT(models::Number<int> &&
               models::StrictNumber<int> &&
               models::RegularStrictNumber<int>);
CONCEPT_ASSERT(models::Number<int, int> &&
               models::StrictNumber<int, int> &&
               models::RegularStrictNumber<int, int>);
CONCEPT_ASSERT(models::Number<int, int, int> &&
               models::StrictNumber<int, int, int> &&
               models::RegularStrictNumber<int, int, int>);

CONCEPT_ASSERT(models::Number<unsigned int> &&
               models::StrictNumber<unsigned int> &&
               models::RegularStrictNumber<unsigned int>);
CONCEPT_ASSERT(models::Number<unsigned int, unsigned int> &&
               models::StrictNumber<unsigned int, unsigned int> &&
               models::RegularStrictNumber<unsigned int, unsigned int>);
CONCEPT_ASSERT(models::Number<unsigned int, unsigned int, unsigned int> &&
               models::StrictNumber<unsigned int, unsigned int, unsigned int> &&
               models::RegularStrictNumber<unsigned int, unsigned int, unsigned int>);

CONCEPT_ASSERT(models::Number<long> &&
               models::StrictNumber<long> &&
               models::RegularStrictNumber<long>);
CONCEPT_ASSERT(models::Number<long, long> &&
               models::StrictNumber<long, long> &&
               models::RegularStrictNumber<long, long>);
CONCEPT_ASSERT(models::Number<long, long, long> &&
               models::StrictNumber<long, long, long> &&
               models::RegularStrictNumber<long, long, long>);

CONCEPT_ASSERT(models::Number<unsigned long> &&
               models::StrictNumber<unsigned long> &&
               models::RegularStrictNumber<unsigned long>);
CONCEPT_ASSERT(models::Number<unsigned long, unsigned long> &&
               models::StrictNumber<unsigned long, unsigned long> &&
               models::RegularStrictNumber<unsigned long, unsigned long>);
CONCEPT_ASSERT(models::Number<unsigned long, unsigned long, unsigned long> &&
               models::StrictNumber<unsigned long, unsigned long, unsigned long> &&
               models::RegularStrictNumber<unsigned long, unsigned long, unsigned long>);

CONCEPT_ASSERT(models::Number<long long> &&
               models::StrictNumber<long long> &&
               models::RegularStrictNumber<long long>);
CONCEPT_ASSERT(models::Number<long long, long long> &&
               models::StrictNumber<long long, long long> &&
               models::RegularStrictNumber<long long, long long>);
CONCEPT_ASSERT(models::Number<long long, long long, long long> &&
               models::StrictNumber<long long, long long, long long> &&
               models::RegularStrictNumber<long long, long long, long long>);

CONCEPT_ASSERT(models::Number<unsigned long long> &&
               models::StrictNumber<unsigned long long> &&
               models::RegularStrictNumber<unsigned long long>);
CONCEPT_ASSERT(models::Number<unsigned long long, unsigned long long> &&
               models::StrictNumber<unsigned long long, unsigned long long> &&
               models::RegularStrictNumber<unsigned long long, unsigned long long>);
CONCEPT_ASSERT(models::Number<unsigned long long, unsigned long long, unsigned long long> &&
               models::StrictNumber<unsigned long long, unsigned long long, unsigned long long> &&
               models::RegularStrictNumber<unsigned long long, unsigned long long, unsigned long long>);

CONCEPT_ASSERT(models::Number<float> &&
               models::StrictNumber<float> &&
               models::RegularStrictNumber<float>);
CONCEPT_ASSERT(models::Number<float, float> &&
               models::StrictNumber<float, float> &&
               models::RegularStrictNumber<float, float>);
CONCEPT_ASSERT(models::Number<float, float, float> &&
               models::StrictNumber<float, float, float> &&
               models::RegularStrictNumber<float, float, float>);

CONCEPT_ASSERT(models::Number<double> &&
               models::StrictNumber<double> &&
               models::RegularStrictNumber<double>);
CONCEPT_ASSERT(models::Number<double, double> &&
               models::StrictNumber<double, double> &&
               models::RegularStrictNumber<double, double>);
CONCEPT_ASSERT(models::Number<double, double, double> &&
               models::StrictNumber<double, double, double> &&
               models::RegularStrictNumber<double, double, double>);

CONCEPT_ASSERT(models::Number<long double> &&
               models::StrictNumber<long double> &&
               models::RegularStrictNumber<long double>);
CONCEPT_ASSERT(models::Number<long double, long double> &&
               models::StrictNumber<long double, long double> &&
               models::RegularStrictNumber<long double, long double>);
CONCEPT_ASSERT(models::Number<long double, long double, long double> &&
               models::StrictNumber<long double, long double, long double> &&
               models::RegularStrictNumber<long double, long double, long double>);

CONCEPT_ASSERT(!models::Number<wchar_t> &&
               !models::StrictNumber<wchar_t> &&
               !models::RegularStrictNumber<wchar_t>);
CONCEPT_ASSERT(!models::Number<wchar_t, wchar_t> &&
               !models::StrictNumber<wchar_t, wchar_t> &&
               !models::RegularStrictNumber<wchar_t, wchar_t>);
CONCEPT_ASSERT(!models::Number<wchar_t, wchar_t, wchar_t> &&
               !models::StrictNumber<wchar_t, wchar_t, wchar_t> &&
               !models::RegularStrictNumber<wchar_t, wchar_t, wchar_t>);

CONCEPT_ASSERT(!models::Number<char16_t> &&
               !models::StrictNumber<char16_t> &&
               !models::RegularStrictNumber<char16_t>);
CONCEPT_ASSERT(!models::Number<char16_t, char16_t> &&
               !models::StrictNumber<char16_t, char16_t> &&
               !models::RegularStrictNumber<char16_t, char16_t>);
CONCEPT_ASSERT(!models::Number<char16_t, char16_t, char16_t> &&
               !models::StrictNumber<char16_t, char16_t, char16_t> &&
               !models::RegularStrictNumber<char16_t, char16_t, char16_t>);

CONCEPT_ASSERT(!models::Number<char32_t> &&
               !models::StrictNumber<char32_t> &&
               !models::RegularStrictNumber<char32_t>);
CONCEPT_ASSERT(!models::Number<char32_t, char32_t> &&
               !models::StrictNumber<char32_t, char32_t> &&
               !models::RegularStrictNumber<char32_t, char32_t>);
CONCEPT_ASSERT(!models::Number<char32_t, char32_t, char32_t> &&
               !models::StrictNumber<char32_t, char32_t, char32_t> &&
               !models::RegularStrictNumber<char32_t, char32_t, char32_t>);

// bools are lepers, make sure that there are no associates...
CONCEPT_ASSERT(!models::Number<bool, int> &&
               !models::StrictNumber<bool, int> &&
               !models::RegularStrictNumber<bool, int>);
CONCEPT_ASSERT(!models::Number<bool, int, double> &&
               !models::StrictNumber<bool, int, double> &&
               !models::RegularStrictNumber<bool, int, double>);
CONCEPT_ASSERT(!models::Number<int, bool> &&
               !models::StrictNumber<int, bool> &&
               !models::RegularStrictNumber<int, bool>);
CONCEPT_ASSERT(!models::Number<int, bool, double> &&
               !models::StrictNumber<int, bool, double> &&
               !models::RegularStrictNumber<int, bool, double>);
CONCEPT_ASSERT(!models::Number<double, bool> &&
               !models::StrictNumber<double, bool> &&
               !models::RegularStrictNumber<double, bool>);
CONCEPT_ASSERT(!models::Number<double, bool, int> &&
               !models::StrictNumber<double, bool, int> &&
               !models::RegularStrictNumber<double, bool, int>);
CONCEPT_ASSERT(!models::Number<bool, char> &&
               !models::StrictNumber<bool, char> &&
               !models::RegularStrictNumber<bool, char>);
CONCEPT_ASSERT(!models::Number<bool, wchar_t> &&
               !models::StrictNumber<bool, wchar_t> &&
               !models::RegularStrictNumber<bool, wchar_t>);
CONCEPT_ASSERT(!models::Number<bool, char16_t> &&
               !models::StrictNumber<bool, char16_t> &&
               !models::RegularStrictNumber<bool, char16_t>);
CONCEPT_ASSERT(!models::Number<bool, char32_t> &&
               !models::StrictNumber<bool, char32_t> &&
               !models::RegularStrictNumber<bool, char32_t>);

// char needs to be lonely
CONCEPT_ASSERT(!models::Number<char, long> &&
               !models::StrictNumber<char, long> &&
               !models::RegularStrictNumber<char, long>);
CONCEPT_ASSERT(!models::Number<char, long, double> &&
               !models::StrictNumber<char, long, double> &&
               !models::RegularStrictNumber<char, long, double>);
CONCEPT_ASSERT(!models::Number<long, char> &&
               !models::StrictNumber<long, char> &&
               !models::RegularStrictNumber<long, char>);
CONCEPT_ASSERT(!models::Number<long, char, double> &&
               !models::StrictNumber<long, char, double> &&
               !models::RegularStrictNumber<long, char, double>);
CONCEPT_ASSERT(!models::Number<double, char> &&
               !models::StrictNumber<double, char> &&
               !models::RegularStrictNumber<double, char>);
CONCEPT_ASSERT(!models::Number<double, char, long> &&
               !models::StrictNumber<double, char, long> &&
               !models::RegularStrictNumber<double, char, long>);
CONCEPT_ASSERT(!models::Number<char, bool> &&
               !models::StrictNumber<char, bool> &&
               !models::RegularStrictNumber<char, bool>);
CONCEPT_ASSERT(!models::Number<char, wchar_t> &&
               !models::StrictNumber<char, wchar_t> &&
               !models::RegularStrictNumber<char, wchar_t>);
CONCEPT_ASSERT(!models::Number<char, char16_t> &&
               !models::StrictNumber<char, char16_t> &&
               !models::RegularStrictNumber<char, char16_t>);
CONCEPT_ASSERT(!models::Number<char, char32_t> &&
               !models::StrictNumber<char, char32_t> &&
               !models::RegularStrictNumber<char, char32_t>);

// wchar_t is also a leper, make sure there are no associates...
CONCEPT_ASSERT(!models::Number<wchar_t, long long> &&
               !models::StrictNumber<wchar_t, long long> &&
               !models::RegularStrictNumber<wchar_t, long long>);
CONCEPT_ASSERT(!models::Number<wchar_t, long long, float> &&
               !models::StrictNumber<wchar_t, long long, float> &&
               !models::RegularStrictNumber<wchar_t, long long, float>);
CONCEPT_ASSERT(!models::Number<long long, wchar_t> &&
               !models::StrictNumber<long long, wchar_t> &&
               !models::RegularStrictNumber<long long, wchar_t>);
CONCEPT_ASSERT(!models::Number<long long, wchar_t, float> &&
               !models::StrictNumber<long long, wchar_t, float> &&
               !models::RegularStrictNumber<long long, wchar_t, float>);
CONCEPT_ASSERT(!models::Number<float, wchar_t> &&
               !models::StrictNumber<float, wchar_t> &&
               !models::RegularStrictNumber<float, wchar_t>);
CONCEPT_ASSERT(!models::Number<float, wchar_t, long long> &&
               !models::StrictNumber<float, wchar_t, long long> &&
               !models::RegularStrictNumber<float, wchar_t, long long>);
CONCEPT_ASSERT(!models::Number<wchar_t, bool> &&
               !models::StrictNumber<wchar_t, bool> &&
               !models::RegularStrictNumber<wchar_t, bool>);
CONCEPT_ASSERT(!models::Number<wchar_t, char> &&
               !models::StrictNumber<wchar_t, char> &&
               !models::RegularStrictNumber<wchar_t, char>);
CONCEPT_ASSERT(!models::Number<wchar_t, char16_t> &&
               !models::StrictNumber<wchar_t, char16_t> &&
               !models::RegularStrictNumber<wchar_t, char16_t>);
CONCEPT_ASSERT(!models::Number<wchar_t, char32_t> &&
               !models::StrictNumber<wchar_t, char32_t> &&
               !models::RegularStrictNumber<wchar_t, char32_t>);

// char16_t can't be associated with anyone either...
CONCEPT_ASSERT(!models::Number<char16_t, short> &&
               !models::StrictNumber<char16_t, short> &&
               !models::RegularStrictNumber<char16_t, short>);
CONCEPT_ASSERT(!models::Number<char16_t, short, long double> &&
               !models::StrictNumber<char16_t, short, long double> &&
               !models::RegularStrictNumber<char16_t, short, long double>);
CONCEPT_ASSERT(!models::Number<short, char16_t> &&
               !models::StrictNumber<short, char16_t> &&
               !models::RegularStrictNumber<short, char16_t>);
CONCEPT_ASSERT(!models::Number<short, char16_t, long double> &&
               !models::StrictNumber<short, char16_t, long double> &&
               !models::RegularStrictNumber<short, char16_t, long double>);
CONCEPT_ASSERT(!models::Number<long double, char16_t> &&
               !models::StrictNumber<long double, char16_t> &&
               !models::RegularStrictNumber<long double, char16_t>);
CONCEPT_ASSERT(!models::Number<long double, char16_t, short> &&
               !models::StrictNumber<long double, char16_t, short> &&
               !models::RegularStrictNumber<long double, char16_t, short>);
CONCEPT_ASSERT(!models::Number<char16_t, bool> &&
               !models::StrictNumber<char16_t, bool> &&
               !models::RegularStrictNumber<char16_t, bool>);
CONCEPT_ASSERT(!models::Number<char16_t, char> &&
               !models::StrictNumber<char16_t, char> &&
               !models::RegularStrictNumber<char16_t, char>);
CONCEPT_ASSERT(!models::Number<char16_t, wchar_t> &&
               !models::StrictNumber<char16_t, wchar_t> &&
               !models::RegularStrictNumber<char16_t, wchar_t>);
CONCEPT_ASSERT(!models::Number<char16_t, char32_t> &&
               !models::StrictNumber<char16_t, char32_t> &&
               !models::RegularStrictNumber<char16_t, char32_t>);

// char32_t is the last fundamental that isn't allowed friends
CONCEPT_ASSERT(!models::Number<char32_t, short> &&
               !models::StrictNumber<char32_t, short> &&
               !models::RegularStrictNumber<char32_t, short>);
CONCEPT_ASSERT(!models::Number<char32_t, short, long double> &&
               !models::StrictNumber<char32_t, short, long double> &&
               !models::RegularStrictNumber<char32_t, short, long double>);
CONCEPT_ASSERT(!models::Number<short, char32_t> &&
               !models::StrictNumber<short, char32_t> &&
               !models::RegularStrictNumber<short, char32_t>);
CONCEPT_ASSERT(!models::Number<short, char32_t, long double> &&
               !models::StrictNumber<short, char32_t, long double> &&
               !models::RegularStrictNumber<short, char32_t, long double>);
CONCEPT_ASSERT(!models::Number<long double, char32_t> &&
               !models::StrictNumber<long double, char32_t> &&
               !models::RegularStrictNumber<long double, char32_t>);
CONCEPT_ASSERT(!models::Number<long double, char32_t, short> &&
               !models::StrictNumber<long double, char32_t, short> &&
               !models::RegularStrictNumber<long double, char32_t, short>);
CONCEPT_ASSERT(!models::Number<char32_t, bool> &&
               !models::StrictNumber<char32_t, bool> &&
               !models::RegularStrictNumber<char32_t, bool>);
CONCEPT_ASSERT(!models::Number<char32_t, char> &&
               !models::StrictNumber<char32_t, char> &&
               !models::RegularStrictNumber<char32_t, char>);
CONCEPT_ASSERT(!models::Number<char32_t, wchar_t> &&
               !models::StrictNumber<char32_t, wchar_t> &&
               !models::RegularStrictNumber<char32_t, wchar_t>);
CONCEPT_ASSERT(!models::Number<char32_t, char16_t> &&
               !models::StrictNumber<char32_t, char16_t> &&
               !models::RegularStrictNumber<char32_t, char16_t>);

CONCEPT_ASSERT(models::Number<short, int> &&
               models::StrictNumber<short, int> &&
               models::RegularStrictNumber<short, int>);
CONCEPT_ASSERT(models::Number<short, long, int> &&
               models::StrictNumber<short, long, int> &&
               models::RegularStrictNumber<short, long, int>);
CONCEPT_ASSERT(models::Number<short, long, int, double> &&
               models::StrictNumber<short, long, int, double> &&
               models::RegularStrictNumber<short, long, int, double>);
CONCEPT_ASSERT(models::Number<int, short> &&
               models::StrictNumber<int, short> &&
               models::RegularStrictNumber<int, short>);
CONCEPT_ASSERT(models::Number<int, short, long> &&
               models::StrictNumber<int, short, long> &&
               models::RegularStrictNumber<int, short, long>);
CONCEPT_ASSERT(models::Number<int, double, short, long> &&
               models::StrictNumber<int, double, short, long> &&
               models::RegularStrictNumber<int, double, short, long>);
CONCEPT_ASSERT(models::Number<long, short> &&
               models::StrictNumber<long, short> &&
               models::RegularStrictNumber<long, short>);
CONCEPT_ASSERT(models::Number<long, int, short> &&
               models::StrictNumber<long, int, short> &&
               models::RegularStrictNumber<long, int, short>);
CONCEPT_ASSERT(models::Number<long, int, double, short> &&
               models::StrictNumber<long, int, double, short> &&
               models::RegularStrictNumber<long, int, double, short>);
CONCEPT_ASSERT(models::Number<double, int> &&
               models::StrictNumber<double, int> &&
               models::RegularStrictNumber<double, int>);
CONCEPT_ASSERT(models::Number<double, long, int> &&
               models::StrictNumber<double, long, int> &&
               models::RegularStrictNumber<double, long, int>);
CONCEPT_ASSERT(models::Number<double, long, int, short> &&
               models::StrictNumber<double, long, int, short> &&
               models::RegularStrictNumber<double, long, int, short>);

CONCEPT_ASSERT(models::Number<float, double> &&
               models::StrictNumber<float, double> &&
               models::RegularStrictNumber<float, double>);
CONCEPT_ASSERT(models::Number<float, double, short> &&
               models::StrictNumber<float, double, short> &&
               models::RegularStrictNumber<float, double, short>);

CONCEPT_ASSERT(models::Number<double, float> &&
               models::StrictNumber<double, float> &&
               models::RegularStrictNumber<double, float>);
CONCEPT_ASSERT(models::Number<double, float, int> &&
               models::StrictNumber<double, float, int> &&
               models::RegularStrictNumber<double, float, int>);

// pointers and references
CONCEPT_ASSERT(!models::Number<long*> &&
               !models::StrictNumber<long*> &&
               !models::RegularStrictNumber<long*>);
CONCEPT_ASSERT(!models::Number<long*, long*> &&
               !models::StrictNumber<long*, long*> &&
               !models::RegularStrictNumber<long*, long*>);
CONCEPT_ASSERT(!models::Number<long*, long*, long*> &&
               !models::StrictNumber<long*, long*, long*> &&
               !models::RegularStrictNumber<long*, long*, long*>);

CONCEPT_ASSERT(!models::Number<const char16_t*> &&
               !models::StrictNumber<const char16_t*> &&
               !models::RegularStrictNumber<const char16_t*>);
CONCEPT_ASSERT(!models::Number<const char16_t*, const char16_t*> &&
               !models::StrictNumber<const char16_t*, const char16_t*> &&
               !models::RegularStrictNumber<const char16_t*, const char16_t*>);
CONCEPT_ASSERT(!models::Number<const char16_t*, const char16_t*, const char16_t*> &&
               !models::StrictNumber<const char16_t*, const char16_t*, const char16_t*> &&
               !models::RegularStrictNumber<const char16_t*, const char16_t*, const char16_t*>);

CONCEPT_ASSERT(!models::Number<long double*, const long double*> &&
               !models::StrictNumber<long double*, const long double*> &&
               !models::RegularStrictNumber<long double*, const long double*>);
CONCEPT_ASSERT(!models::Number<const long double*, long double*> &&
               !models::StrictNumber<const long double*, long double*> &&
               !models::RegularStrictNumber<const long double*, long double*>);
CONCEPT_ASSERT(!models::Number<const long double*, const long double*> &&
               !models::StrictNumber<const long double*, const long double*> &&
               !models::RegularStrictNumber<const long double*, const long double*>);
CONCEPT_ASSERT(!models::Number<char*, char*, const char*> &&
               !models::StrictNumber<char*, char*, const char*> &&
               !models::RegularStrictNumber<char*, char*, const char*>);
CONCEPT_ASSERT(!models::Number<char*, const char*, char*> &&
               !models::StrictNumber<char*, const char*, char*> &&
               !models::RegularStrictNumber<char*, const char*, char*>);
CONCEPT_ASSERT(!models::Number<char*, const char*, const char*> &&
               !models::StrictNumber<char*, const char*, const char*> &&
               !models::RegularStrictNumber<char*, const char*, const char*>);
CONCEPT_ASSERT(!models::Number<const char*, char*, char*> &&
               !models::StrictNumber<const char*, char*, char*> &&
               !models::RegularStrictNumber<const char*, char*, char*>);
CONCEPT_ASSERT(!models::Number<const char*, char*, const char*> &&
               !models::StrictNumber<const char*, char*, const char*> &&
               !models::RegularStrictNumber<const char*, char*, const char*>);
CONCEPT_ASSERT(!models::Number<const char*, const char*, char*> &&
               !models::StrictNumber<const char*, const char*, char*> &&
               !models::RegularStrictNumber<const char*, const char*, char*>);
CONCEPT_ASSERT(!models::Number<const char*, const char*, const char*> &&
               !models::StrictNumber<const char*, const char*, const char*> &&
               !models::RegularStrictNumber<const char*, const char*, const char*>);

CONCEPT_ASSERT(!models::Number<int&> &&
               !models::StrictNumber<int&> &&
               !models::RegularStrictNumber<int&>);
CONCEPT_ASSERT(!models::Number<int&, int&> &&
               !models::StrictNumber<int&, int&> &&
               !models::RegularStrictNumber<int&, int&>);
CONCEPT_ASSERT(!models::Number<int&, int&, int&> &&
               !models::StrictNumber<int&, int&, int&> &&
               !models::RegularStrictNumber<int&, int&, int&>);

CONCEPT_ASSERT(!models::Number<const char32_t&> &&
               !models::StrictNumber<const char32_t&> &&
               !models::RegularStrictNumber<const char32_t&>);
CONCEPT_ASSERT(!models::Number<const char32_t&, const char32_t&> &&
               !models::StrictNumber<const char32_t&, const char32_t&> &&
               !models::RegularStrictNumber<const char32_t&, const char32_t&>);
CONCEPT_ASSERT(!models::Number<const char32_t&, const char32_t&, const char32_t&> &&
               !models::StrictNumber<const char32_t&, const char32_t&, const char32_t&> &&
               !models::RegularStrictNumber<const char32_t&, const char32_t&, const char32_t&>);

CONCEPT_ASSERT(!models::Number<char32_t&, const char32_t&> &&
               !models::StrictNumber<char32_t&, const char32_t&> &&
               !models::RegularStrictNumber<char32_t&, const char32_t&>);
CONCEPT_ASSERT(!models::Number<const char32_t&, char32_t&> &&
               !models::StrictNumber<const char32_t&, char32_t&> &&
               !models::RegularStrictNumber<const char32_t&, char32_t&>);
CONCEPT_ASSERT(!models::Number<const char32_t&, const char32_t&> &&
               !models::StrictNumber<const char32_t&, const char32_t&> &&
               !models::RegularStrictNumber<const char32_t&, const char32_t&>);

CONCEPT_ASSERT(!models::Number<short&, short&, const short&> &&
               !models::StrictNumber<short&, short&, const short&> &&
               !models::RegularStrictNumber<short&, short&, const short&>);
CONCEPT_ASSERT(!models::Number<short&, const short&, short&> &&
               !models::StrictNumber<short&, const short&, short&> &&
               !models::RegularStrictNumber<short&, const short&, short&>);
CONCEPT_ASSERT(!models::Number<short&, const short&, const short&> &&
               !models::StrictNumber<short&, const short&, const short&> &&
               !models::RegularStrictNumber<short&, const short&, const short&>);
CONCEPT_ASSERT(!models::Number<const short&, short&, short&> &&
               !models::StrictNumber<const short&, short&, short&> &&
               !models::RegularStrictNumber<const short&, short&, short&>);
CONCEPT_ASSERT(!models::Number<const short&, short&, const short&> &&
               !models::StrictNumber<const short&, short&, const short&> &&
               !models::RegularStrictNumber<const short&, short&, const short&>);
CONCEPT_ASSERT(!models::Number<const short&, const short&, short&> &&
               !models::StrictNumber<const short&, const short&, short&> &&
               !models::RegularStrictNumber<const short&, const short&, short&>);
CONCEPT_ASSERT(!models::Number<const short&, const short&, const short&> &&
               !models::StrictNumber<const short&, const short&, const short&> &&
               !models::RegularStrictNumber<const short&, const short&, const short&>);

CONCEPT_ASSERT(!models::Number<int&&> &&
               !models::StrictNumber<int&&> &&
               !models::RegularStrictNumber<int&&>);
CONCEPT_ASSERT(!models::Number<int&&, int&&> &&
               !models::StrictNumber<int&&, int&&> &&
               !models::RegularStrictNumber<int&&, int&&>);
CONCEPT_ASSERT(!models::Number<int&&, int&&, int&&> &&
               !models::StrictNumber<int&&, int&&, int&&> &&
               !models::RegularStrictNumber<int&&, int&&, int&&>);

CONCEPT_ASSERT(!models::Number<int, long*> &&
               !models::StrictNumber<int, long*> &&
               !models::RegularStrictNumber<int, long*>);
CONCEPT_ASSERT(!models::Number<int, long*, double> &&
               !models::StrictNumber<int, long*, double> &&
               !models::RegularStrictNumber<int, long*, double>);
CONCEPT_ASSERT(!models::Number<int, double, long*> &&
               !models::StrictNumber<int, double, long*> &&
               !models::RegularStrictNumber<int, double, long*>);

CONCEPT_ASSERT(!models::Number<int, const long*> &&
               !models::StrictNumber<int, const long*> &&
               !models::RegularStrictNumber<int, const long*>);
CONCEPT_ASSERT(!models::Number<int, const long*, double> &&
               !models::StrictNumber<int, const long*, double> &&
               !models::RegularStrictNumber<int, const long*, double>);
CONCEPT_ASSERT(!models::Number<int, double, const long*> &&
               !models::StrictNumber<int, double, const long*> &&
               !models::RegularStrictNumber<int, double, const long*>);

CONCEPT_ASSERT(!models::Number<int*, float> &&
               !models::StrictNumber<int*, float> &&
               !models::RegularStrictNumber<int*, float>);
CONCEPT_ASSERT(!models::Number<int*, float, wchar_t> &&
               !models::StrictNumber<int*, float, wchar_t> &&
               !models::RegularStrictNumber<int*, float, wchar_t>);
CONCEPT_ASSERT(!models::Number<int*, wchar_t, float> &&
               !models::StrictNumber<int*, wchar_t, float> &&
               !models::RegularStrictNumber<int*, wchar_t, float>);

CONCEPT_ASSERT(!models::Number<const int*, float> &&
               !models::StrictNumber<const int*, float> &&
               !models::RegularStrictNumber<const int*, float>);
CONCEPT_ASSERT(!models::Number<const int*, float, wchar_t> &&
               !models::StrictNumber<const int*, float, wchar_t> &&
               !models::RegularStrictNumber<const int*, float, wchar_t>);
CONCEPT_ASSERT(!models::Number<const int*, wchar_t, float> &&
               !models::StrictNumber<const int*, wchar_t, float> &&
               !models::RegularStrictNumber<const int*, wchar_t, float>);

// complex is the reason Number exists
// we should thoroughly test that it is weak, and anything it combines with is weak at best
CONCEPT_ASSERT(models::Number<std::complex<double>> &&
               !models::StrictNumber<std::complex<double>> &&
               !models::RegularStrictNumber<std::complex<double>>);
CONCEPT_ASSERT(models::Number<std::complex<double>, std::complex<double>> &&
               !models::StrictNumber<std::complex<double>, std::complex<double>> &&
               !models::RegularStrictNumber<std::complex<double>, std::complex<double>>);
CONCEPT_ASSERT(models::Number<std::complex<double>, std::complex<double>, std::complex<double>> &&
               !models::StrictNumber<std::complex<double>, std::complex<double>, std::complex<double>> &&
               !models::RegularStrictNumber<std::complex<double>, std::complex<double>, std::complex<double>>);

/*CONCEPT_ASSERT(models::Number<complex<double>, complex<int>> &&
               !models::StrictNumber<complex<double>, complex<int>> &&
               !models::RegularStrictNumber<complex<double>, complex<int>, complex<float>>);
CONCEPT_ASSERT(models::Number<complex<double>, complex<int>, complex<float>> &&
               !models::StrictNumber<complex<double>, complex<int>, complex<float>> &&
               !models::RegularStrictNumber<complex<double>, complex<int>, complex<float>>);
CONCEPT_ASSERT(models::Number<complex<double>, int> &&
               !models::StrictNumber<complex<double>, int> &&
               !models::RegularStrictNumber<complex<double>, int>);
CONCEPT_ASSERT(models::Number<complex<double>, int, double> &&
               !models::StrictNumber<complex<double>, int, double> &&
               !models::RegularStrictNumber<complex<double>, int, double>);
CONCEPT_ASSERT(models::Number<int, complex<double>> &&
               !models::StrictNumber<int, complex<double>> &&
               !models::RegularStrictNumber<int, complex<double>>);
CONCEPT_ASSERT(models::Number<int, complex<double>, double> &&
               !models::StrictNumber<int, complex<double>, double> &&
               !models::RegularStrictNumber<int, complex<double>, double>);*/

// test something that's genuinely not related to arithmetic types at all
CONCEPT_ASSERT(!models::Number<std::vector<int>> &&
               !models::StrictNumber<std::vector<int>> &&
               !models::RegularStrictNumber<std::vector<int>>);

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

CONCEPT_ASSERT(models::Number<Fixed_point> &&
               !models::StrictNumber<Fixed_point> &&
               !models::RegularStrictNumber<Fixed_point>);
CONCEPT_ASSERT(models::Number<Fixed_point, Fixed_point> &&
               !models::StrictNumber<Fixed_point, Fixed_point> &&
               !models::RegularStrictNumber<Fixed_point, Fixed_point>);
CONCEPT_ASSERT(models::Number<Fixed_point, Fixed_point, Fixed_point> &&
               !models::StrictNumber<Fixed_point, Fixed_point, Fixed_point> &&
               !models::RegularStrictNumber<Fixed_point, Fixed_point, Fixed_point>);
CONCEPT_ASSERT(!models::Number<Fixed_point, int> &&
               !models::StrictNumber<Fixed_point, int> &&
               !models::RegularStrictNumber<Fixed_point, int>);
CONCEPT_ASSERT(!models::Number<Fixed_point, int, double> &&
               !models::StrictNumber<Fixed_point, int, double> &&
               !models::RegularStrictNumber<Fixed_point, int, double>);
CONCEPT_ASSERT(!models::Number<int, Fixed_point> &&
               !models::StrictNumber<int, Fixed_point> &&
               !models::RegularStrictNumber<int, Fixed_point>);
CONCEPT_ASSERT(!models::Number<int, Fixed_point, double> &&
               !models::StrictNumber<int, Fixed_point, double> &&
               !models::RegularStrictNumber<int, Fixed_point, double>);
CONCEPT_ASSERT(!models::Number<Fixed_point, std::complex<double>> &&
               !models::StrictNumber<Fixed_point, std::complex<double>> &&
               !models::RegularStrictNumber<Fixed_point, std::complex<double>>);

STL2_OPEN_NAMESPACE {
template <>
struct common_type<Uint128, std::uint64_t> {
   using type = Uint128;
};

template <>
struct common_type<std::uint64_t, Uint128> : public common_type<Uint128, std::uint64_t> {
};
} STL2_CLOSE_NAMESPACE

CONCEPT_ASSERT(models::Number<Uint128> &&
               models::RegularNumber<Uint128> &&
               models::StrictNumber<Uint128> &&
               models::RegularStrictNumber<Uint128>);
CONCEPT_ASSERT(models::Number<Uint128, Uint128> &&
               models::RegularNumber<Uint128, Uint128> &&
               models::StrictNumber<Uint128, Uint128> &&
               models::RegularStrictNumber<Uint128, Uint128>);
CONCEPT_ASSERT(models::Number<Uint128, Uint128, Uint128> &&
               models::RegularNumber<Uint128, Uint128, Uint128> &&
               models::StrictNumber<Uint128, Uint128, Uint128> &&
               models::RegularStrictNumber<Uint128, Uint128, Uint128>);
CONCEPT_ASSERT(!models::Number<Uint128, std::int64_t> &&
               !models::RegularNumber<Uint128, std::int64_t> &&
               !models::StrictNumber<Uint128, std::int64_t> &&
               !models::RegularStrictNumber<Uint128, std::int64_t>);
CONCEPT_ASSERT(!models::Number<std::int64_t, Uint128> &&
               !models::RegularNumber<std::int64_t, Uint128> &&
               !models::StrictNumber<std::int64_t, Uint128> &&
               !models::RegularStrictNumber<std::uint64_t, Uint128>);
CONCEPT_ASSERT(models::Number<Uint128, std::uint64_t> &&
               models::RegularNumber<Uint128, std::uint64_t> &&
               models::StrictNumber<Uint128, std::uint64_t> &&
               models::RegularStrictNumber<Uint128, std::uint64_t>);
CONCEPT_ASSERT(!models::Number<std::uint64_t, Uint128> &&
               !models::RegularNumber<std::uint64_t, Uint128> &&
               !models::StrictNumber<std::uint64_t, Uint128> &&
               !models::RegularStrictNumber<std::uint64_t, Uint128>);
CONCEPT_ASSERT(!models::Number<Uint128, double> &&
               !models::RegularNumber<Uint128, double> &&
               !models::StrictNumber<Uint128, double> &&
               !models::RegularStrictNumber<Uint128, double>);
//CONCEPT_ASSERT(models::Number<cmcstl2::Int128

int main()
{
}
