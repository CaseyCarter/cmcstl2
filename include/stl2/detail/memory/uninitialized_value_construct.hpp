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
#ifndef STL2_DETAIL_MEMORY_UNINITIALIZED_VALUE_CONSTRUCT_HPP
#define STL2_DETAIL_MEMORY_UNINITIALIZED_VALUE_CONSTRUCT_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/memory/concepts.hpp>
#include <stl2/detail/memory/construct_at.hpp>
#include <stl2/detail/memory/destroy.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// uninitialized_value_construct [uninitialized.construct.value]
	//
	struct __uninitialized_value_construct_fn : private __niebloid {
		template<_NoThrowForwardIterator I, _NoThrowSentinel<I> S>
		requires default_initializable<iter_value_t<I>>
		I operator()(I first, S last) const {
			auto guard = detail::destroy_guard{first};
			for (; first != last; ++first) {
				__stl2::__construct_at(*first);
			}
			guard.release();
			return first;
		}

		template<_NoThrowForwardRange Rng>
		requires default_initializable<iter_value_t<iterator_t<Rng>>>
		safe_iterator_t<Rng> operator()(Rng&& rng) const {
			return (*this)(begin(rng), end(rng));
		}
	};

	inline constexpr __uninitialized_value_construct_fn uninitialized_value_construct{};

	///////////////////////////////////////////////////////////////////////////
	// uninitialized_value_construct_n [uninitialized.construct.value]
	//
	struct __uninitialized_value_construct_n_fn : private __niebloid {
		template<_NoThrowForwardIterator I>
		requires default_initializable<iter_value_t<I>>
		I operator()(I first, iter_difference_t<I> n) const {
			return uninitialized_value_construct(
				counted_iterator{first, n},
				default_sentinel).base();
		}
	};

	inline constexpr __uninitialized_value_construct_n_fn uninitialized_value_construct_n{};
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_MEMORY_UNINITIALIZED_VALUE_CONSTRUCT_HPP
