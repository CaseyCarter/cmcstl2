// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter
//  Copyright Christopher Di Bella
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include "../validate.hpp"

#if VALIDATE_STL2
#include <stl2/detail/concepts/numeric.hpp>
#endif

#include "../../simple_test.hpp"
#include <complex>
#include <cstddef>
#include <functional>
#include <stl2/detail/functional/arithmetic.hpp>

#include "models.hpp"

using ranges::ext::Magma;
using ranges::ext::Semigroup;
using ranges::ext::Monoid;

STL2_CONCEPT_TEMPLATE_FUNDAMENTAL_DEFINITION(Magma)
STL2_CONCEPT_TEMPLATE_FUNDAMENTAL_DEFINITION(Semigroup)
STL2_CONCEPT_TEMPLATE_FUNDAMENTAL_DEFINITION(Monoid)

STL2_CONCEPT_TEMPLATE_REFERENCE_DEFINITION(Magma)
STL2_CONCEPT_TEMPLATE_REFERENCE_DEFINITION(Semigroup)
STL2_CONCEPT_TEMPLATE_REFERENCE_DEFINITION(Monoid)

STL2_CONCEPT_TEMPLATE_RVALUE_REFERENCE_DEFINITION(Magma)
STL2_CONCEPT_TEMPLATE_RVALUE_REFERENCE_DEFINITION(Semigroup)
STL2_CONCEPT_TEMPLATE_RVALUE_REFERENCE_DEFINITION(Monoid)

template<class T, class U>
requires Monoid<ranges::ext::plus, T, U>
void f() {}

int main()
{
	// Some basic checks
	CHECK_FUNDAMENTALS(Magma, ranges::identity{});
	CHECK_FUNDAMENTALS(Semigroup, ranges::identity{});
	CHECK_FUNDAMENTALS(Monoid, std::logical_not{});

	CHECK_REFERENCE_TO_FUNDAMENTALS(Magma, ranges::identity{});
	CHECK_REFERENCE_TO_FUNDAMENTALS(Semigroup, ranges::identity{});
	CHECK_REFERENCE_TO_FUNDAMENTALS(Monoid, std::logical_not{});

	CHECK_RVALUE_REFERENCE_TO_FUNDAMENTALS(Magma, ranges::identity{});
	CHECK_RVALUE_REFERENCE_TO_FUNDAMENTALS(Semigroup, ranges::identity{});
	CHECK_RVALUE_REFERENCE_TO_FUNDAMENTALS(Monoid, std::logical_not{});

	CHECK_USER_TYPE(/* nothing */, Magma);
	CHECK_USER_TYPE(/* nothing */, Semigroup);
	CHECK_USER_TYPE(not, Monoid);

	CHECK_REFERENCE_TO_USER_TYPE(Magma, ranges::identity{});
	CHECK_REFERENCE_TO_USER_TYPE(Semigroup, ranges::identity{});
	CHECK_REFERENCE_TO_USER_TYPE(Monoid, std::logical_not{});

	using ranges::ext::minus;
	CONCEPT_ASSERT(Magma<minus, int, int>);
	// CONCEPT_ASSERT(not Semigroup<minus, int, int>);
	CONCEPT_ASSERT(not Monoid<minus, int, int>);

	CONCEPT_ASSERT(Magma<minus, int, double>);
	// CONCEPT_ASSERT(not Semigroup<minus, int, double>);
	CONCEPT_ASSERT(not Monoid<minus, int, double>);

	CONCEPT_ASSERT(Magma<minus, double, int>);
	// CONCEPT_ASSERT(not Semigroup<minus, double, int>);
	CONCEPT_ASSERT(not Monoid<minus, double, int>);

	CONCEPT_ASSERT(not Magma<minus, std::string, std::string>);
	CONCEPT_ASSERT(not Semigroup<minus, std::string, std::string>);
	CONCEPT_ASSERT(not Monoid<minus, std::string, std::string>);

	using ranges::ext::quotient;
	CONCEPT_ASSERT(Magma<quotient, int, int>);
	// CONCEPT_ASSERT(not Semigroup<quotient, int, int>);
	CONCEPT_ASSERT(not Monoid<quotient, int, int>);

	CONCEPT_ASSERT(Magma<quotient, int, double>);
	// CONCEPT_ASSERT(not Semigroup<quotient, int, double>);
	CONCEPT_ASSERT(not Monoid<quotient, int, double>);

	CONCEPT_ASSERT(Magma<quotient, double, int>);
	// CONCEPT_ASSERT(not Semigroup<quotient, double, int>);
	CONCEPT_ASSERT(not Monoid<quotient, double, int>);

	CONCEPT_ASSERT(not Magma<quotient, std::string, std::string>);
	CONCEPT_ASSERT(not Semigroup<quotient, std::string, std::string>);
	CONCEPT_ASSERT(not Monoid<quotient, std::string, std::string>);

	using ranges::ext::modulus;
	CONCEPT_ASSERT(Magma<modulus, int, int>);
	// CONCEPT_ASSERT(not Semigroup<modulus, int, int>);
	CONCEPT_ASSERT(not Monoid<modulus, int, int>);

	CONCEPT_ASSERT(not Magma<modulus, int, double>);
	// CONCEPT_ASSERT(not Semigroup<modulus, int, double>);
	CONCEPT_ASSERT(not Monoid<modulus, int, double>);

	CONCEPT_ASSERT(not Magma<modulus, double, int>);
	// CONCEPT_ASSERT(not Semigroup<modulus, double, int>);
	CONCEPT_ASSERT(not Monoid<modulus, double, int>);

	CONCEPT_ASSERT(not Magma<modulus, std::string, std::string>);
	// CONCEPT_ASSERT(not Semigroup<modulus, std::string, std::string>);
	CONCEPT_ASSERT(not Monoid<modulus, std::string, std::string>);

	// after hundreds of tests proving semigroups are *not* monoids, we finally get a few that show
	// some semigroups that are ususally monoids!
	using ranges::ext::plus;
	CONCEPT_ASSERT(Magma<plus, int, int>);
	CONCEPT_ASSERT(Semigroup<plus, int, int>);
	CONCEPT_ASSERT(Monoid<plus, int, int>);

	CONCEPT_ASSERT(Magma<plus, int, double>);
	CONCEPT_ASSERT(Semigroup<plus, int, double>);
	CONCEPT_ASSERT(Monoid<plus, int, double>);

	CONCEPT_ASSERT(Magma<plus, double, int>);
	CONCEPT_ASSERT(Semigroup<plus, double, int>);
	CONCEPT_ASSERT(Monoid<plus, double, int>);

	CONCEPT_ASSERT(Magma<plus, std::string, std::string>);
	CONCEPT_ASSERT(Semigroup<plus, std::string, std::string>);
	CONCEPT_ASSERT(Monoid<plus, std::string, std::string>);

	using ranges::ext::product;
	CONCEPT_ASSERT(Magma<product, int, int>);
	CONCEPT_ASSERT(Semigroup<product, int, int>);
	CONCEPT_ASSERT(Monoid<product, int, int>);

	CONCEPT_ASSERT(Magma<product, int, double>);
	CONCEPT_ASSERT(Semigroup<product, int, double>);
	CONCEPT_ASSERT(Monoid<product, int, double>);

	CONCEPT_ASSERT(Magma<product, double, int>);
	CONCEPT_ASSERT(Semigroup<product, double, int>);
	CONCEPT_ASSERT(Monoid<product, double, int>);

	CONCEPT_ASSERT(not Magma<product, std::string, std::string>);
	CONCEPT_ASSERT(not Semigroup<product, std::string, std::string>);
	CONCEPT_ASSERT(not Monoid<product, std::string, std::string>);

	return ::test_result();
}
