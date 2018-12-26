// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Christopher Di Bella 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_MEMORY_UNINITIALIZED_FILL_HPP
#define STL2_DETAIL_MEMORY_UNINITIALIZED_FILL_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/memory/concepts.hpp>
#include <stl2/detail/memory/construct_at.hpp>
#include <stl2/detail/memory/destroy.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// uninitialized_fill [uninitialized.fill]
	//
	struct __uninitialized_fill_fn : private __niebloid {
		template<_NoThrowForwardIterator I, _NoThrowSentinel<I> S, class T>
		requires Constructible<iter_value_t<I>, const T&>
		I operator()(I first, S last, const T& x) const {
			auto guard = detail::destroy_guard{first};
			for (; first != last; ++first) {
				__stl2::__construct_at(*first, x);
			}
			guard.release();
			return first;
		}

		template<_NoThrowForwardRange R, class T>
		requires Constructible<iter_value_t<iterator_t<R>>, const T&>
		safe_iterator_t<R> operator()(R&& r, const T& x) const {
			return (*this)(begin(r), end(r), x);
		}
	};

	inline constexpr __uninitialized_fill_fn uninitialized_fill {};

	///////////////////////////////////////////////////////////////////////////
	// uninitialized_fill_n [uninitialized.fill]
	//
	struct __uninitialized_fill_n_fn : private __niebloid {
		template<_NoThrowForwardIterator I, class T>
		requires Constructible<iter_value_t<I>, const T&>
		I operator()(I first, const iter_difference_t<I> n, const T& x) const {
			return uninitialized_fill(
				counted_iterator{std::move(first), n},
				default_sentinel{}, x).base();
		}
	};

	inline constexpr __uninitialized_fill_n_fn uninitialized_fill_n {};
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_MEMORY_UNINITIALIZED_FILL_HPP
