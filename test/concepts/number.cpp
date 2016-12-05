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

#if VALIDATE_RANGES // guessing VALIDATER_RANGES is for range-v3
#elif VALIDATE_STL2
#include <stl2/detail/concepts/number.hpp>
#endif

#include <complex> // TODO: replace with <stl2/detail/complex.hpp>
#include <string>
#include <vector>

namespace ranges = __stl2;

// fundamental types
CONCEPT_ASSERT(!models::WeakNumber<bool> &&
               !models::Number<bool> &&
               !models::RegularNumber<bool>);
CONCEPT_ASSERT(!models::WeakNumber<bool, bool> &&
               !models::Number<bool, bool> &&
               !models::RegularNumber<bool, bool>);
CONCEPT_ASSERT(!models::WeakNumber<bool, bool, bool> &&
               !models::Number<bool, bool, bool> &&
               !models::RegularNumber<bool, bool, bool>);

CONCEPT_ASSERT(!models::WeakNumber<char> &&
               !models::Number<char, char> &&
               !models::RegularNumber<char>);
CONCEPT_ASSERT(!models::WeakNumber<char> &&
               !models::Number<char, char> &&
               !models::RegularNumber<char, char>);
CONCEPT_ASSERT(!models::WeakNumber<char, char, char> &&
               !models::Number<char, char, char> &&
               !models::RegularNumber<char, char, char>);

CONCEPT_ASSERT(models::WeakNumber<signed char> &&
               models::Number<signed char> &&
               models::RegularNumber<signed char>);
CONCEPT_ASSERT(models::WeakNumber<signed char, signed char> &&
               models::Number<signed char, signed char> &&
               models::RegularNumber<signed char, signed char>);
CONCEPT_ASSERT(models::WeakNumber<signed char, signed char, signed char> &&
               models::Number<signed char, signed char, signed char> &&
               models::RegularNumber<signed char, signed char, signed char>);

CONCEPT_ASSERT(models::WeakNumber<unsigned char> &&
               models::Number<unsigned char> &&
               models::RegularNumber<unsigned char>);
CONCEPT_ASSERT(models::WeakNumber<unsigned char, unsigned char> &&
               models::Number<unsigned char, unsigned char> &&
               models::RegularNumber<unsigned char, unsigned char>);
CONCEPT_ASSERT(models::WeakNumber<unsigned char, unsigned char, unsigned char> &&
               models::Number<unsigned char, unsigned char, unsigned char> &&
               models::RegularNumber<unsigned char, unsigned char, unsigned char>);

CONCEPT_ASSERT(models::WeakNumber<short> &&
               models::Number<short> &&
               models::RegularNumber<short>);
CONCEPT_ASSERT(models::WeakNumber<short, short> &&
               models::Number<short, short> &&
               models::RegularNumber<short, short>);
CONCEPT_ASSERT(models::WeakNumber<short, short, short> &&
               models::Number<short, short, short> &&
               models::RegularNumber<short, short, short>);

CONCEPT_ASSERT(models::WeakNumber<unsigned short> &&
               models::Number<unsigned short> &&
               models::RegularNumber<unsigned short>);
CONCEPT_ASSERT(models::WeakNumber<unsigned short, unsigned short> &&
               models::Number<unsigned short, unsigned short> &&
               models::RegularNumber<unsigned short, unsigned short>);
CONCEPT_ASSERT(models::WeakNumber<unsigned short, unsigned short, unsigned short> &&
               models::Number<unsigned short, unsigned short, unsigned short> &&
               models::RegularNumber<unsigned short, unsigned short, unsigned short>);

CONCEPT_ASSERT(models::WeakNumber<int> &&
               models::Number<int> &&
               models::RegularNumber<int>);
CONCEPT_ASSERT(models::WeakNumber<int, int> &&
               models::Number<int, int> &&
               models::RegularNumber<int, int>);
CONCEPT_ASSERT(models::WeakNumber<int, int, int> &&
               models::Number<int, int, int> &&
               models::RegularNumber<int, int, int>);

CONCEPT_ASSERT(models::WeakNumber<unsigned int> &&
               models::Number<unsigned int> &&
               models::RegularNumber<unsigned int>);
CONCEPT_ASSERT(models::WeakNumber<unsigned int, unsigned int> &&
               models::Number<unsigned int, unsigned int> &&
               models::RegularNumber<unsigned int, unsigned int>);
CONCEPT_ASSERT(models::WeakNumber<unsigned int, unsigned int, unsigned int> &&
               models::Number<unsigned int, unsigned int, unsigned int> &&
               models::RegularNumber<unsigned int, unsigned int, unsigned int>);

CONCEPT_ASSERT(models::WeakNumber<long> &&
               models::Number<long> &&
               models::RegularNumber<long>);
CONCEPT_ASSERT(models::WeakNumber<long, long> &&
               models::Number<long, long> &&
               models::RegularNumber<long, long>);
CONCEPT_ASSERT(models::WeakNumber<long, long, long> &&
               models::Number<long, long, long> &&
               models::RegularNumber<long, long, long>);

CONCEPT_ASSERT(models::WeakNumber<unsigned long> &&
               models::Number<unsigned long> &&
               models::RegularNumber<unsigned long>);
CONCEPT_ASSERT(models::WeakNumber<unsigned long, unsigned long> &&
               models::Number<unsigned long, unsigned long> &&
               models::RegularNumber<unsigned long, unsigned long>);
CONCEPT_ASSERT(models::WeakNumber<unsigned long, unsigned long, unsigned long> &&
               models::Number<unsigned long, unsigned long, unsigned long> &&
               models::RegularNumber<unsigned long, unsigned long, unsigned long>);

CONCEPT_ASSERT(models::WeakNumber<long long> &&
               models::Number<long long> &&
               models::RegularNumber<long long>);
CONCEPT_ASSERT(models::WeakNumber<long long, long long> &&
               models::Number<long long, long long> &&
               models::RegularNumber<long long, long long>);
CONCEPT_ASSERT(models::WeakNumber<long long, long long, long long> &&
               models::Number<long long, long long, long long> &&
               models::RegularNumber<long long, long long, long long>);

CONCEPT_ASSERT(models::WeakNumber<unsigned long long> &&
               models::Number<unsigned long long> &&
               models::RegularNumber<unsigned long long>);
CONCEPT_ASSERT(models::WeakNumber<unsigned long long, unsigned long long> &&
               models::Number<unsigned long long, unsigned long long> &&
               models::RegularNumber<unsigned long long, unsigned long long>);
CONCEPT_ASSERT(models::WeakNumber<unsigned long long, unsigned long long, unsigned long long> &&
               models::Number<unsigned long long, unsigned long long, unsigned long long> &&
               models::RegularNumber<unsigned long long, unsigned long long, unsigned long long>);

CONCEPT_ASSERT(models::WeakNumber<float> &&
               models::Number<float> &&
               models::RegularNumber<float>);
CONCEPT_ASSERT(models::WeakNumber<float, float> &&
               models::Number<float, float> &&
               models::RegularNumber<float, float>);
CONCEPT_ASSERT(models::WeakNumber<float, float, float> &&
               models::Number<float, float, float> &&
               models::RegularNumber<float, float, float>);

CONCEPT_ASSERT(models::WeakNumber<double> &&
               models::Number<double> &&
               models::RegularNumber<double>);
CONCEPT_ASSERT(models::WeakNumber<double, double> &&
               models::Number<double, double> &&
               models::RegularNumber<double, double>);
CONCEPT_ASSERT(models::WeakNumber<double, double, double> &&
               models::Number<double, double, double> &&
               models::RegularNumber<double, double, double>);

CONCEPT_ASSERT(models::WeakNumber<long double> &&
               models::Number<long double> &&
               models::RegularNumber<long double>);
CONCEPT_ASSERT(models::WeakNumber<long double, long double> &&
               models::Number<long double, long double> &&
               models::RegularNumber<long double, long double>);
CONCEPT_ASSERT(models::WeakNumber<long double, long double, long double> &&
               models::Number<long double, long double, long double> &&
               models::RegularNumber<long double, long double, long double>);

CONCEPT_ASSERT(!models::WeakNumber<wchar_t> &&
               !models::Number<wchar_t> &&
               !models::RegularNumber<wchar_t>);
CONCEPT_ASSERT(!models::WeakNumber<wchar_t, wchar_t> &&
               !models::Number<wchar_t, wchar_t> &&
               !models::RegularNumber<wchar_t, wchar_t>);
CONCEPT_ASSERT(!models::WeakNumber<wchar_t, wchar_t, wchar_t> &&
               !models::Number<wchar_t, wchar_t, wchar_t> &&
               !models::RegularNumber<wchar_t, wchar_t, wchar_t>);

CONCEPT_ASSERT(!models::WeakNumber<char16_t> &&
               !models::Number<char16_t> &&
               !models::RegularNumber<char16_t>);
CONCEPT_ASSERT(!models::WeakNumber<char16_t, char16_t> &&
               !models::Number<char16_t, char16_t> &&
               !models::RegularNumber<char16_t, char16_t>);
CONCEPT_ASSERT(!models::WeakNumber<char16_t, char16_t, char16_t> &&
               !models::Number<char16_t, char16_t, char16_t> &&
               !models::RegularNumber<char16_t, char16_t, char16_t>);

CONCEPT_ASSERT(!models::WeakNumber<char32_t> &&
               !models::Number<char32_t> &&
               !models::RegularNumber<char32_t>);
CONCEPT_ASSERT(!models::WeakNumber<char32_t, char32_t> &&
               !models::Number<char32_t, char32_t> &&
               !models::RegularNumber<char32_t, char32_t>);
CONCEPT_ASSERT(!models::WeakNumber<char32_t, char32_t, char32_t> &&
               !models::Number<char32_t, char32_t, char32_t> &&
               !models::RegularNumber<char32_t, char32_t, char32_t>);

// bools are lepers, make sure that there are no associates...
CONCEPT_ASSERT(!models::WeakNumber<bool, int> &&
               !models::Number<bool, int> &&
               !models::RegularNumber<bool, int>);
CONCEPT_ASSERT(!models::WeakNumber<bool, int, double> &&
               !models::Number<bool, int, double> &&
               !models::RegularNumber<bool, int, double>);
CONCEPT_ASSERT(!models::WeakNumber<int, bool> &&
               !models::Number<int, bool> &&
               !models::RegularNumber<int, bool>);
CONCEPT_ASSERT(!models::WeakNumber<int, bool, double> &&
               !models::Number<int, bool, double> &&
               !models::RegularNumber<int, bool, double>);
CONCEPT_ASSERT(!models::WeakNumber<double, bool> &&
               !models::Number<double, bool> &&
               !models::RegularNumber<double, bool>);
CONCEPT_ASSERT(!models::WeakNumber<double, bool, int> &&
               !models::Number<double, bool, int> &&
               !models::RegularNumber<double, bool, int>);
CONCEPT_ASSERT(!models::WeakNumber<bool, char> &&
               !models::Number<bool, char> &&
               !models::RegularNumber<bool, char>);
CONCEPT_ASSERT(!models::WeakNumber<bool, wchar_t> &&
               !models::Number<bool, wchar_t> &&
               !models::RegularNumber<bool, wchar_t>);
CONCEPT_ASSERT(!models::WeakNumber<bool, char16_t> &&
               !models::Number<bool, char16_t> &&
               !models::RegularNumber<bool, char16_t>);
CONCEPT_ASSERT(!models::WeakNumber<bool, char32_t> &&
               !models::Number<bool, char32_t> &&
               !models::RegularNumber<bool, char32_t>);

// char needs to be lonely
CONCEPT_ASSERT(!models::WeakNumber<char, long> &&
               !models::Number<char, long> &&
               !models::RegularNumber<char, long>);
CONCEPT_ASSERT(!models::WeakNumber<char, long, double> &&
               !models::Number<char, long, double> &&
               !models::RegularNumber<char, long, double>);
CONCEPT_ASSERT(!models::WeakNumber<long, char> &&
               !models::Number<long, char> &&
               !models::RegularNumber<long, char>);
CONCEPT_ASSERT(!models::WeakNumber<long, char, double> &&
               !models::Number<long, char, double> &&
               !models::RegularNumber<long, char, double>);
CONCEPT_ASSERT(!models::WeakNumber<double, char> &&
               !models::Number<double, char> &&
               !models::RegularNumber<double, char>);
CONCEPT_ASSERT(!models::WeakNumber<double, char, long> &&
               !models::Number<double, char, long> &&
               !models::RegularNumber<double, char, long>);
CONCEPT_ASSERT(!models::WeakNumber<char, bool> &&
               !models::Number<char, bool> &&
               !models::RegularNumber<char, bool>);
CONCEPT_ASSERT(!models::WeakNumber<char, wchar_t> &&
               !models::Number<char, wchar_t> &&
               !models::RegularNumber<char, wchar_t>);
CONCEPT_ASSERT(!models::WeakNumber<char, char16_t> &&
               !models::Number<char, char16_t> &&
               !models::RegularNumber<char, char16_t>);
CONCEPT_ASSERT(!models::WeakNumber<char, char32_t> &&
               !models::Number<char, char32_t> &&
               !models::RegularNumber<char, char32_t>);

// wchar_t is also a leper, make sure there are no associates...
CONCEPT_ASSERT(!models::WeakNumber<wchar_t, long long> &&
               !models::Number<wchar_t, long long> &&
               !models::RegularNumber<wchar_t, long long>);
CONCEPT_ASSERT(!models::WeakNumber<wchar_t, long long, float> &&
               !models::Number<wchar_t, long long, float> &&
               !models::RegularNumber<wchar_t, long long, float>);
CONCEPT_ASSERT(!models::WeakNumber<long long, wchar_t> &&
               !models::Number<long long, wchar_t> &&
               !models::RegularNumber<long long, wchar_t>);
CONCEPT_ASSERT(!models::WeakNumber<long long, wchar_t, float> &&
               !models::Number<long long, wchar_t, float> &&
               !models::RegularNumber<long long, wchar_t, float>);
CONCEPT_ASSERT(!models::WeakNumber<float, wchar_t> &&
               !models::Number<float, wchar_t> &&
               !models::RegularNumber<float, wchar_t>);
CONCEPT_ASSERT(!models::WeakNumber<float, wchar_t, long long> &&
               !models::Number<float, wchar_t, long long> &&
               !models::RegularNumber<float, wchar_t, long long>);
CONCEPT_ASSERT(!models::WeakNumber<wchar_t, bool> &&
               !models::Number<wchar_t, bool> &&
               !models::RegularNumber<wchar_t, bool>);
CONCEPT_ASSERT(!models::WeakNumber<wchar_t, char> &&
               !models::Number<wchar_t, char> &&
               !models::RegularNumber<wchar_t, char>);
CONCEPT_ASSERT(!models::WeakNumber<wchar_t, char16_t> &&
               !models::Number<wchar_t, char16_t> &&
               !models::RegularNumber<wchar_t, char16_t>);
CONCEPT_ASSERT(!models::WeakNumber<wchar_t, char32_t> &&
               !models::Number<wchar_t, char32_t> &&
               !models::RegularNumber<wchar_t, char32_t>);

// char16_t can't be associated with anyone either...
CONCEPT_ASSERT(!models::WeakNumber<char16_t, short> &&
               !models::Number<char16_t, short> &&
               !models::RegularNumber<char16_t, short>);
CONCEPT_ASSERT(!models::WeakNumber<char16_t, short, long double> &&
               !models::Number<char16_t, short, long double> &&
               !models::RegularNumber<char16_t, short, long double>);
CONCEPT_ASSERT(!models::WeakNumber<short, char16_t> &&
               !models::Number<short, char16_t> &&
               !models::RegularNumber<short, char16_t>);
CONCEPT_ASSERT(!models::WeakNumber<short, char16_t, long double> &&
               !models::Number<short, char16_t, long double> &&
               !models::RegularNumber<short, char16_t, long double>);
CONCEPT_ASSERT(!models::WeakNumber<long double, char16_t> &&
               !models::Number<long double, char16_t> &&
               !models::RegularNumber<long double, char16_t>);
CONCEPT_ASSERT(!models::WeakNumber<long double, char16_t, short> &&
               !models::Number<long double, char16_t, short> &&
               !models::RegularNumber<long double, char16_t, short>);
CONCEPT_ASSERT(!models::WeakNumber<char16_t, bool> &&
               !models::Number<char16_t, bool> &&
               !models::RegularNumber<char16_t, bool>);
CONCEPT_ASSERT(!models::WeakNumber<char16_t, char> &&
               !models::Number<char16_t, char> &&
               !models::RegularNumber<char16_t, char>);
CONCEPT_ASSERT(!models::WeakNumber<char16_t, wchar_t> &&
               !models::Number<char16_t, wchar_t> &&
               !models::RegularNumber<char16_t, wchar_t>);
CONCEPT_ASSERT(!models::WeakNumber<char16_t, char32_t> &&
               !models::Number<char16_t, char32_t> &&
               !models::RegularNumber<char16_t, char32_t>);

// char32_t is the last fundamental that isn't allowed friends
CONCEPT_ASSERT(!models::WeakNumber<char32_t, short> &&
               !models::Number<char32_t, short> &&
               !models::RegularNumber<char32_t, short>);
CONCEPT_ASSERT(!models::WeakNumber<char32_t, short, long double> &&
               !models::Number<char32_t, short, long double> &&
               !models::RegularNumber<char32_t, short, long double>);
CONCEPT_ASSERT(!models::WeakNumber<short, char32_t> &&
               !models::Number<short, char32_t> &&
               !models::RegularNumber<short, char32_t>);
CONCEPT_ASSERT(!models::WeakNumber<short, char32_t, long double> &&
               !models::Number<short, char32_t, long double> &&
               !models::RegularNumber<short, char32_t, long double>);
CONCEPT_ASSERT(!models::WeakNumber<long double, char32_t> &&
               !models::Number<long double, char32_t> &&
               !models::RegularNumber<long double, char32_t>);
CONCEPT_ASSERT(!models::WeakNumber<long double, char32_t, short> &&
               !models::Number<long double, char32_t, short> &&
               !models::RegularNumber<long double, char32_t, short>);
CONCEPT_ASSERT(!models::WeakNumber<char32_t, bool> &&
               !models::Number<char32_t, bool> &&
               !models::RegularNumber<char32_t, bool>);
CONCEPT_ASSERT(!models::WeakNumber<char32_t, char> &&
               !models::Number<char32_t, char> &&
               !models::RegularNumber<char32_t, char>);
CONCEPT_ASSERT(!models::WeakNumber<char32_t, wchar_t> &&
               !models::Number<char32_t, wchar_t> &&
               !models::RegularNumber<char32_t, wchar_t>);
CONCEPT_ASSERT(!models::WeakNumber<char32_t, char16_t> &&
               !models::Number<char32_t, char16_t> &&
               !models::RegularNumber<char32_t, char16_t>);

CONCEPT_ASSERT(models::WeakNumber<short, int> &&
               models::Number<short, int> &&
               models::RegularNumber<short, int>);
CONCEPT_ASSERT(models::WeakNumber<short, long, int> &&
               models::Number<short, long, int> &&
               models::RegularNumber<short, long, int>);
CONCEPT_ASSERT(models::WeakNumber<short, long, int, double> &&
               models::Number<short, long, int, double> &&
               models::RegularNumber<short, long, int, double>);
CONCEPT_ASSERT(models::WeakNumber<int, short> &&
               models::Number<int, short> &&
               models::RegularNumber<int, short>);
CONCEPT_ASSERT(models::WeakNumber<int, short, long> &&
               models::Number<int, short, long> &&
               models::RegularNumber<int, short, long>);
CONCEPT_ASSERT(models::WeakNumber<int, double, short, long> &&
               models::Number<int, double, short, long> &&
               models::RegularNumber<int, double, short, long>);
CONCEPT_ASSERT(models::WeakNumber<long, short> &&
               models::Number<long, short> &&
               models::RegularNumber<long, short>);
CONCEPT_ASSERT(models::WeakNumber<long, int, short> &&
               models::Number<long, int, short> &&
               models::RegularNumber<long, int, short>);
CONCEPT_ASSERT(models::WeakNumber<long, int, double, short> &&
               models::Number<long, int, double, short> &&
               models::RegularNumber<long, int, double, short>);
CONCEPT_ASSERT(models::WeakNumber<double, int> &&
               models::Number<double, int> &&
               models::RegularNumber<double, int>);
CONCEPT_ASSERT(models::WeakNumber<double, long, int> &&
               models::Number<double, long, int> &&
               models::RegularNumber<double, long, int>);
CONCEPT_ASSERT(models::WeakNumber<double, long, int, short> &&
               models::Number<double, long, int, short> &&
               models::RegularNumber<double, long, int, short>);

CONCEPT_ASSERT(models::WeakNumber<float, double> &&
               models::Number<float, double> &&
               models::RegularNumber<float, double>);
CONCEPT_ASSERT(models::WeakNumber<float, double, short> &&
               models::Number<float, double, short> &&
               models::RegularNumber<float, double, short>);

CONCEPT_ASSERT(models::WeakNumber<double, float> &&
               models::Number<double, float> &&
               models::RegularNumber<double, float>);
CONCEPT_ASSERT(models::WeakNumber<double, float, int> &&
               models::Number<double, float, int> &&
               models::RegularNumber<double, float, int>);

// pointers and references
CONCEPT_ASSERT(!models::WeakNumber<long*> &&
               !models::Number<long*> &&
               !models::RegularNumber<long*>);
CONCEPT_ASSERT(!models::WeakNumber<long*, long*> &&
               !models::Number<long*, long*> &&
               !models::RegularNumber<long*, long*>);
CONCEPT_ASSERT(!models::WeakNumber<long*, long*, long*> &&
               !models::Number<long*, long*, long*> &&
               !models::RegularNumber<long*, long*, long*>);

CONCEPT_ASSERT(!models::WeakNumber<const char16_t*> &&
               !models::Number<const char16_t*> &&
               !models::RegularNumber<const char16_t*>);
CONCEPT_ASSERT(!models::WeakNumber<const char16_t*, const char16_t*> &&
               !models::Number<const char16_t*, const char16_t*> &&
               !models::RegularNumber<const char16_t*, const char16_t*>);
CONCEPT_ASSERT(!models::WeakNumber<const char16_t*, const char16_t*, const char16_t*> &&
               !models::Number<const char16_t*, const char16_t*, const char16_t*> &&
               !models::RegularNumber<const char16_t*, const char16_t*, const char16_t*>);

CONCEPT_ASSERT(!models::WeakNumber<long double*, const long double*> &&
               !models::Number<long double*, const long double*> &&
               !models::RegularNumber<long double*, const long double*>);
CONCEPT_ASSERT(!models::WeakNumber<const long double*, long double*> &&
               !models::Number<const long double*, long double*> &&
               !models::RegularNumber<const long double*, long double*>);
CONCEPT_ASSERT(!models::WeakNumber<const long double*, const long double*> &&
               !models::Number<const long double*, const long double*> &&
               !models::RegularNumber<const long double*, const long double*>);
CONCEPT_ASSERT(!models::WeakNumber<char*, char*, const char*> &&
               !models::Number<char*, char*, const char*> &&
               !models::RegularNumber<char*, char*, const char*>);
CONCEPT_ASSERT(!models::WeakNumber<char*, const char*, char*> &&
               !models::Number<char*, const char*, char*> &&
               !models::RegularNumber<char*, const char*, char*>);
CONCEPT_ASSERT(!models::WeakNumber<char*, const char*, const char*> &&
               !models::Number<char*, const char*, const char*> &&
               !models::RegularNumber<char*, const char*, const char*>);
CONCEPT_ASSERT(!models::WeakNumber<const char*, char*, char*> &&
               !models::Number<const char*, char*, char*> &&
               !models::RegularNumber<const char*, char*, char*>);
CONCEPT_ASSERT(!models::WeakNumber<const char*, char*, const char*> &&
               !models::Number<const char*, char*, const char*> &&
               !models::RegularNumber<const char*, char*, const char*>);
CONCEPT_ASSERT(!models::WeakNumber<const char*, const char*, char*> &&
               !models::Number<const char*, const char*, char*> &&
               !models::RegularNumber<const char*, const char*, char*>);
CONCEPT_ASSERT(!models::WeakNumber<const char*, const char*, const char*> &&
               !models::Number<const char*, const char*, const char*> &&
               !models::RegularNumber<const char*, const char*, const char*>);

CONCEPT_ASSERT(!models::WeakNumber<int&> &&
               !models::Number<int&> &&
               !models::RegularNumber<int&>);
CONCEPT_ASSERT(!models::WeakNumber<int&, int&> &&
               !models::Number<int&, int&> &&
               !models::RegularNumber<int&, int&>);
CONCEPT_ASSERT(!models::WeakNumber<int&, int&, int&> &&
               !models::Number<int&, int&, int&> &&
               !models::RegularNumber<int&, int&, int&>);

CONCEPT_ASSERT(!models::WeakNumber<const char32_t&> &&
               !models::Number<const char32_t&> &&
               !models::RegularNumber<const char32_t&>);
CONCEPT_ASSERT(!models::WeakNumber<const char32_t&, const char32_t&> &&
               !models::Number<const char32_t&, const char32_t&> &&
               !models::RegularNumber<const char32_t&, const char32_t&>);
CONCEPT_ASSERT(!models::WeakNumber<const char32_t&, const char32_t&, const char32_t&> &&
               !models::Number<const char32_t&, const char32_t&, const char32_t&> &&
               !models::RegularNumber<const char32_t&, const char32_t&, const char32_t&>);

CONCEPT_ASSERT(!models::WeakNumber<char32_t&, const char32_t&> &&
               !models::Number<char32_t&, const char32_t&> &&
               !models::RegularNumber<char32_t&, const char32_t&>);
CONCEPT_ASSERT(!models::WeakNumber<const char32_t&, char32_t&> &&
               !models::Number<const char32_t&, char32_t&> &&
               !models::RegularNumber<const char32_t&, char32_t&>);
CONCEPT_ASSERT(!models::WeakNumber<const char32_t&, const char32_t&> &&
               !models::Number<const char32_t&, const char32_t&> &&
               !models::RegularNumber<const char32_t&, const char32_t&>);

CONCEPT_ASSERT(!models::WeakNumber<short&, short&, const short&> &&
               !models::Number<short&, short&, const short&> &&
               !models::RegularNumber<short&, short&, const short&>);
CONCEPT_ASSERT(!models::WeakNumber<short&, const short&, short&> &&
               !models::Number<short&, const short&, short&> &&
               !models::RegularNumber<short&, const short&, short&>);
CONCEPT_ASSERT(!models::WeakNumber<short&, const short&, const short&> &&
               !models::Number<short&, const short&, const short&> &&
               !models::RegularNumber<short&, const short&, const short&>);
CONCEPT_ASSERT(!models::WeakNumber<const short&, short&, short&> &&
               !models::Number<const short&, short&, short&> &&
               !models::RegularNumber<const short&, short&, short&>);
CONCEPT_ASSERT(!models::WeakNumber<const short&, short&, const short&> &&
               !models::Number<const short&, short&, const short&> &&
               !models::RegularNumber<const short&, short&, const short&>);
CONCEPT_ASSERT(!models::WeakNumber<const short&, const short&, short&> &&
               !models::Number<const short&, const short&, short&> &&
               !models::RegularNumber<const short&, const short&, short&>);
CONCEPT_ASSERT(!models::WeakNumber<const short&, const short&, const short&> &&
               !models::Number<const short&, const short&, const short&> &&
               !models::RegularNumber<const short&, const short&, const short&>);

CONCEPT_ASSERT(!models::WeakNumber<int&&> &&
               !models::Number<int&&> &&
               !models::RegularNumber<int&&>);
CONCEPT_ASSERT(!models::WeakNumber<int&&, int&&> &&
               !models::Number<int&&, int&&> &&
               !models::RegularNumber<int&&, int&&>);
CONCEPT_ASSERT(!models::WeakNumber<int&&, int&&, int&&> &&
               !models::Number<int&&, int&&, int&&> &&
               !models::RegularNumber<int&&, int&&, int&&>);

CONCEPT_ASSERT(!models::WeakNumber<int, long*> &&
               !models::Number<int, long*> &&
               !models::RegularNumber<int, long*>);
CONCEPT_ASSERT(!models::WeakNumber<int, long*, double> &&
               !models::Number<int, long*, double> &&
               !models::RegularNumber<int, long*, double>);
CONCEPT_ASSERT(!models::WeakNumber<int, double, long*> &&
               !models::Number<int, double, long*> &&
               !models::RegularNumber<int, double, long*>);

CONCEPT_ASSERT(!models::WeakNumber<int, const long*> &&
               !models::Number<int, const long*> &&
               !models::RegularNumber<int, const long*>);
CONCEPT_ASSERT(!models::WeakNumber<int, const long*, double> &&
               !models::Number<int, const long*, double> &&
               !models::RegularNumber<int, const long*, double>);
CONCEPT_ASSERT(!models::WeakNumber<int, double, const long*> &&
               !models::Number<int, double, const long*> &&
               !models::RegularNumber<int, double, const long*>);

CONCEPT_ASSERT(!models::WeakNumber<int*, float> &&
               !models::Number<int*, float> &&
               !models::RegularNumber<int*, float>);
CONCEPT_ASSERT(!models::WeakNumber<int*, float, wchar_t> &&
               !models::Number<int*, float, wchar_t> &&
               !models::RegularNumber<int*, float, wchar_t>);
CONCEPT_ASSERT(!models::WeakNumber<int*, wchar_t, float> &&
               !models::Number<int*, wchar_t, float> &&
               !models::RegularNumber<int*, wchar_t, float>);

CONCEPT_ASSERT(!models::WeakNumber<const int*, float> &&
               !models::Number<const int*, float> &&
               !models::RegularNumber<const int*, float>);
CONCEPT_ASSERT(!models::WeakNumber<const int*, float, wchar_t> &&
               !models::Number<const int*, float, wchar_t> &&
               !models::RegularNumber<const int*, float, wchar_t>);
CONCEPT_ASSERT(!models::WeakNumber<const int*, wchar_t, float> &&
               !models::Number<const int*, wchar_t, float> &&
               !models::RegularNumber<const int*, wchar_t, float>);

// complex is the reason WeakNumber exists
// we should thoroughly test that it is weak, and anything it combines with is weak at best
CONCEPT_ASSERT(models::WeakNumber<std::complex<double>> &&
               !models::Number<std::complex<double>> &&
               !models::RegularNumber<std::complex<double>>);
CONCEPT_ASSERT(models::WeakNumber<std::complex<double>, std::complex<double>> &&
               !models::Number<std::complex<double>, std::complex<double>> &&
               !models::RegularNumber<std::complex<double>, std::complex<double>>);
CONCEPT_ASSERT(models::WeakNumber<std::complex<double>, std::complex<double>, std::complex<double>> &&
               !models::Number<std::complex<double>, std::complex<double>, std::complex<double>> &&
               !models::RegularNumber<std::complex<double>, std::complex<double>, std::complex<double>>);

/*CONCEPT_ASSERT(models::WeakNumber<complex<double>, complex<int>> &&
               !models::Number<complex<double>, complex<int>> &&
               !models::RegularNumber<complex<double>, complex<int>, complex<float>>);
CONCEPT_ASSERT(models::WeakNumber<complex<double>, complex<int>, complex<float>> &&
               !models::Number<complex<double>, complex<int>, complex<float>> &&
               !models::RegularNumber<complex<double>, complex<int>, complex<float>>);
CONCEPT_ASSERT(models::WeakNumber<complex<double>, int> &&
               !models::Number<complex<double>, int> &&
               !models::RegularNumber<complex<double>, int>);
CONCEPT_ASSERT(models::WeakNumber<complex<double>, int, double> &&
               !models::Number<complex<double>, int, double> &&
               !models::RegularNumber<complex<double>, int, double>);
CONCEPT_ASSERT(models::WeakNumber<int, complex<double>> &&
               !models::Number<int, complex<double>> &&
               !models::RegularNumber<int, complex<double>>);
CONCEPT_ASSERT(models::WeakNumber<int, complex<double>, double> &&
               !models::Number<int, complex<double>, double> &&
               !models::RegularNumber<int, complex<double>, double>);*/

// test something that's genuinely not related to arithmetic types at all
CONCEPT_ASSERT(!models::WeakNumber<std::vector<int>> &&
               !models::Number<std::vector<int>> &&
               !models::RegularNumber<std::vector<int>>);

// custom WeakNumber compound type
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

CONCEPT_ASSERT(models::WeakNumber<Fixed_point> &&
               !models::Number<Fixed_point> &&
               !models::RegularNumber<Fixed_point>);
CONCEPT_ASSERT(models::WeakNumber<Fixed_point, Fixed_point> &&
               !models::Number<Fixed_point, Fixed_point> &&
               !models::RegularNumber<Fixed_point, Fixed_point>);
CONCEPT_ASSERT(models::WeakNumber<Fixed_point, Fixed_point, Fixed_point> &&
               !models::Number<Fixed_point, Fixed_point, Fixed_point> &&
               !models::RegularNumber<Fixed_point, Fixed_point, Fixed_point>);
CONCEPT_ASSERT(!models::WeakNumber<Fixed_point, int> &&
               !models::Number<Fixed_point, int> &&
               !models::RegularNumber<Fixed_point, int>);
CONCEPT_ASSERT(!models::WeakNumber<Fixed_point, int, double> &&
               !models::Number<Fixed_point, int, double> &&
               !models::RegularNumber<Fixed_point, int, double>);
CONCEPT_ASSERT(!models::WeakNumber<int, Fixed_point> &&
               !models::Number<int, Fixed_point> &&
               !models::RegularNumber<int, Fixed_point>);
CONCEPT_ASSERT(!models::WeakNumber<int, Fixed_point, double> &&
               !models::Number<int, Fixed_point, double> &&
               !models::RegularNumber<int, Fixed_point, double>);
CONCEPT_ASSERT(!models::WeakNumber<Fixed_point, std::complex<double>> &&
               !models::Number<Fixed_point, std::complex<double>> &&
               !models::RegularNumber<Fixed_point, std::complex<double>>);

// custom Number compound type
namespace {
   class Uint128 {
   public:
      Uint128() noexcept = default;
      explicit Uint128(const bool b) noexcept;
      explicit Uint128(const std::uint64_t l, const std::int64_t u = 0) noexcept;

      Uint128& operator=(const std::uint64_t o) noexcept;

      Uint128& operator+=(const Uint128& i) noexcept;
      Uint128& operator-=(const Uint128& i) noexcept;
      Uint128& operator*=(const Uint128& i) noexcept;
      Uint128& operator/=(const Uint128& i) noexcept;

      Uint128& operator+=(const std::uint64_t&) noexcept;
      Uint128& operator-=(const std::uint64_t&) noexcept;
      Uint128& operator*=(const std::uint64_t&) noexcept;
      Uint128& operator/=(const std::uint64_t&) noexcept;

      Uint128& operator++() noexcept;
      Uint128& operator--() noexcept;
      Uint128 operator++(int) noexcept;
      Uint128 operator--(int) noexcept;
   private:
      std::uint64_t lower_{0};
      std::uint64_t upper_{0};
   };

   Uint128 operator+(const Uint128&) noexcept { return {}; }
   Uint128 operator-(const Uint128&) noexcept { return {}; }

   Uint128 operator+(const Uint128&, Uint128) noexcept { return {}; }
   Uint128 operator-(const Uint128&, Uint128) noexcept { return {}; }
   Uint128 operator*(const Uint128&, Uint128) noexcept { return {}; }
   Uint128 operator/(const Uint128&, Uint128) noexcept { return {}; }

   Uint128 operator+(const Uint128&, std::uint64_t) noexcept { return {}; }
   Uint128 operator-(const Uint128&, std::uint64_t) noexcept { return {}; }
   Uint128 operator*(const Uint128&, std::uint64_t) noexcept { return {}; }
   Uint128 operator/(const Uint128&, std::uint64_t) noexcept { return {}; }

   Uint128 operator+(std::uint64_t, const Uint128&) noexcept { return {}; }
   Uint128 operator-(std::uint64_t, const Uint128&) noexcept { return {}; }
   Uint128 operator*(std::uint64_t, const Uint128&) noexcept { return {}; }
   Uint128 operator/(std::uint64_t, const Uint128&) noexcept { return {}; }

   bool operator==(const Uint128&, const Uint128&) noexcept { return true; }
   bool operator!=(const Uint128&, const Uint128&) noexcept { return true; }
   bool operator<(const Uint128&, const Uint128&) noexcept { return true; }
   bool operator<=(const Uint128&, const Uint128&) noexcept { return true; }
   bool operator>=(const Uint128&, const Uint128&) noexcept { return true; }
   bool operator>(const Uint128&, const Uint128&) noexcept { return true; }

   bool operator==(const Uint128&, std::uint64_t) noexcept { return true; }
   bool operator!=(const Uint128&, std::uint64_t) noexcept { return true; }
   bool operator<(const Uint128&, std::uint64_t) noexcept { return true; }
   bool operator<=(const Uint128&, std::uint64_t) noexcept { return true; }
   bool operator>=(const Uint128&, std::uint64_t) noexcept { return true; }
   bool operator>(const Uint128&, std::uint64_t) noexcept { return true; }

   bool operator==(std::uint64_t, const Uint128&) noexcept { return true; }
   bool operator!=(std::uint64_t, const Uint128&) noexcept { return true; }
   bool operator<(std::uint64_t, const Uint128&) noexcept { return true; }
   bool operator<=(std::uint64_t, const Uint128&) noexcept { return true; }
   bool operator>=(std::uint64_t, const Uint128&) noexcept { return true; }
   bool operator>(std::uint64_t, const Uint128&) noexcept { return true; }
}

STL2_OPEN_NAMESPACE {
template <>
struct common_type<Uint128, std::uint64_t> {
   using type = Uint128;
};

template <>
struct common_type<std::uint64_t, Uint128> : public common_type<Uint128, std::uint64_t> {
};
} STL2_CLOSE_NAMESPACE

CONCEPT_ASSERT(models::WeakNumber<Uint128> &&
               models::RegularWeakNumber<Uint128> &&
               models::Number<Uint128> &&
               models::RegularNumber<Uint128>);
CONCEPT_ASSERT(models::WeakNumber<Uint128, Uint128> &&
               models::RegularWeakNumber<Uint128, Uint128> &&
               models::Number<Uint128, Uint128> &&
               models::RegularNumber<Uint128, Uint128>);
CONCEPT_ASSERT(models::WeakNumber<Uint128, Uint128, Uint128> &&
               models::RegularWeakNumber<Uint128, Uint128, Uint128> &&
               models::Number<Uint128, Uint128, Uint128> &&
               models::RegularNumber<Uint128, Uint128, Uint128>);
CONCEPT_ASSERT(!models::WeakNumber<Uint128, std::int64_t> &&
               !models::RegularWeakNumber<Uint128, std::int64_t> &&
               !models::Number<Uint128, std::int64_t> &&
               !models::RegularNumber<Uint128, std::int64_t>);
CONCEPT_ASSERT(!models::WeakNumber<std::int64_t, Uint128> &&
               !models::RegularWeakNumber<std::int64_t, Uint128> &&
               !models::Number<std::int64_t, Uint128> &&
               !models::RegularNumber<std::uint64_t, Uint128>);
CONCEPT_ASSERT(models::WeakNumber<Uint128, std::uint64_t> &&
               models::RegularWeakNumber<Uint128, std::uint64_t> &&
               models::Number<Uint128, std::uint64_t> &&
               models::RegularNumber<Uint128, std::uint64_t>);
CONCEPT_ASSERT(!models::WeakNumber<std::uint64_t, Uint128> &&
               !models::RegularWeakNumber<std::uint64_t, Uint128> &&
               !models::Number<std::uint64_t, Uint128> &&
               !models::RegularNumber<std::uint64_t, Uint128>);
CONCEPT_ASSERT(!models::WeakNumber<Uint128, double> &&
               !models::RegularWeakNumber<Uint128, double> &&
               !models::Number<Uint128, double> &&
               !models::RegularNumber<Uint128, double>);
//CONCEPT_ASSERT(models::WeakNumber<cmcstl2::Int128

int main()
{
}
