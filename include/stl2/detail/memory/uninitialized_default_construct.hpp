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
#ifndef STL2_DETAIL_MEMORY_UNINITIALIZED_DEFAULT_CONSTRUCT_HPP
#define STL2_DETAIL_MEMORY_UNINITIALIZED_DEFAULT_CONSTRUCT_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/memory/concepts.hpp>
#include <stl2/detail/memory/construct_at.hpp>
#include <stl2/detail/memory/destroy.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// uninitialized_default_construct [uninitialized.construct.default]
	//
	struct __uninitialized_default_construct_fn : private __niebloid {
		template<_NoThrowForwardIterator I, _NoThrowSentinel<I> S>
		requires default_initializable<iter_value_t<I>>
		I operator()(I first, S last) const {
			auto guard = detail::destroy_guard{first};
			for (; first != last; ++first) {
				__stl2::__default_construct_at(*first);
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

	inline constexpr __uninitialized_default_construct_fn uninitialized_default_construct{};

	///////////////////////////////////////////////////////////////////////////
	// uninitialized_default_construct_n [Extension]
	//
	struct __uninitialized_default_construct_n_fn : private __niebloid {
		template<_NoThrowForwardIterator I>
		requires default_initializable<iter_value_t<I>>
		I operator()(I first, iter_difference_t<I> n) const {
			return uninitialized_default_construct(
				counted_iterator{std::move(first), n},
				default_sentinel).base();
		}
	};

	inline constexpr __uninitialized_default_construct_n_fn uninitialized_default_construct_n{};
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_MEMORY_UNINITIALIZED_DEFAULT_CONSTRUCT_HPP
