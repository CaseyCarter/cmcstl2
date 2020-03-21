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
#ifndef STL2_DETAIL_CONCEPTS_NUMERIC_HPP
#define STL2_DETAIL_CONCEPTS_NUMERIC_HPP

#include <stl2/detail/concepts/numeric/magma.hpp>
#include <stl2/detail/concepts/numeric/semigroup.hpp>
#include <stl2/detail/concepts/numeric/monoid.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/fwd.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template<class BOp, class I1, class I2, class O>
		META_CONCEPT IndirectMagma =
			Readable<I1> &&
			Readable<I2> &&
			Writable<O, indirect_result_t<BOp&, I1, I2>> &&
			Magma<BOp&, iter_value_t<I1>&, iter_value_t<I2>&> &&
			Magma<BOp&, iter_value_t<I1>&, iter_reference_t<I2>&> &&
			Magma<BOp&, iter_reference_t<I1>, iter_value_t<I2>&> &&
			Magma<BOp&, iter_reference_t<I1>, iter_reference_t<I2>> &&
			Magma<BOp&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;

		template<class BOp, class I1, class I2, class O>
		META_CONCEPT IndirectSemigroup = IndirectMagma<BOp, I1, I2, O> &&
			Semigroup<BOp&, iter_value_t<I1>&, iter_value_t<I2>&> &&
			Semigroup<BOp&, iter_value_t<I1>&, iter_reference_t<I2>&> &&
			Semigroup<BOp&, iter_reference_t<I1>, iter_value_t<I2>&> &&
			Semigroup<BOp&, iter_reference_t<I1>, iter_reference_t<I2>> &&
			Semigroup<BOp&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;

		template<class BOp, class I1, class I2, class O>
		META_CONCEPT IndirectMonoid = IndirectSemigroup<BOp, I1, I2, O> &&
			Monoid<BOp&, iter_value_t<I1>&, iter_value_t<I2>&> &&
			Monoid<BOp&, iter_value_t<I1>&, iter_reference_t<I2>&> &&
			Monoid<BOp&, iter_reference_t<I1>, iter_value_t<I2>&> &&
			Monoid<BOp&, iter_reference_t<I1>, iter_reference_t<I2>> &&
			Monoid<BOp&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_CONCEPTS_NUMERIC_HPP
