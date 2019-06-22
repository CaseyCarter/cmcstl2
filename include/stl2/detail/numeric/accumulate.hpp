// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_NUMERIC_REDUCE_HPP
#define STL2_DETAIL_NUMERIC_REDUCE_HPP

#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/numeric/magma.hpp>
#include <stl2/detail/concepts/numeric/monoid.hpp>
#include <stl2/detail/concepts/numeric/semigroup.hpp>
#include <stl2/detail/functional/arithmetic/plus.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/numeric_traits/identity.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/range/dangling.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		// review for accumulate.hpp begins here
		template<class I, class T>
		struct accumulate_result : __in_value_result<I, T> {};

		struct __accumulate_fn : private __niebloid {
			template<InputIterator I, Sentinel<I> S, Movable T, class Proj = identity,
				IndirectMagma<projected<I, Proj>, T*, T*> BOp = __stl2::ext::plus>
			constexpr accumulate_result<I, T>
			operator()(I first, S last, T init, BOp bop = {}, Proj proj = {}) const {
				for (; first != last; ++first) {
					init = __stl2::invoke(bop, std::move(init), __stl2::invoke(proj, *first));
				}

				return {std::move(first), std::move(init)};
			}

			template<InputRange R, Movable T, class Proj = identity,
				IndirectMagma<projected<iterator_t<R>, Proj>, T*, T*> BOp = __stl2::ext::plus>
			constexpr accumulate_result<safe_iterator_t<R>, T>
			operator()(R&& r, T init, BOp bop = {}, Proj proj = {}) const {
				return (*this)(begin(r), end(r), std::move(init), std::ref(bop), std::ref(proj));
			}
		};

		inline constexpr __accumulate_fn accumulate {};
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_NUMERIC_REDUCE_HPP
