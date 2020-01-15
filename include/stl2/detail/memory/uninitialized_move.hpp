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
#ifndef STL2_DETAIL_MEMORY_UNINITIALIZED_MOVE_HPP
#define STL2_DETAIL_MEMORY_UNINITIALIZED_MOVE_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/memory/concepts.hpp>
#include <stl2/detail/memory/construct_at.hpp>
#include <stl2/detail/memory/destroy.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// uninitialized_move [uninitialized.move]
	//
	template<class I, class O>
	using uninitialized_move_result = __in_out_result<I, O>;

	struct __uninitialized_move_fn : private __niebloid {
		template<input_iterator I, sentinel_for<I> S1,
			_NoThrowForwardIterator O, _NoThrowSentinel<O> S2>
		requires constructible_from<iter_value_t<O>, iter_rvalue_reference_t<I>>
		uninitialized_move_result<I, O>
		operator()(I ifirst, S1 ilast, O ofirst, S2 olast) const {
			auto guard = detail::destroy_guard{ofirst};
			for (; ifirst != ilast && ofirst != olast; (void) ++ifirst, (void) ++ofirst) {
				__stl2::__construct_at(*ofirst, iter_move(ifirst));
			}
			guard.release();
			return {std::move(ifirst), std::move(ofirst)};
		}

		template<input_range IR, _NoThrowForwardRange OR>
		requires constructible_from<iter_value_t<iterator_t<OR>>,
		                       iter_rvalue_reference_t<iterator_t<IR>>>
		uninitialized_move_result<safe_iterator_t<IR>, safe_iterator_t<OR>>
		operator()(IR&& in, OR&& out) const {
			return (*this)(begin(in), end(in), begin(out), end(out));
		}
	};

	inline constexpr __uninitialized_move_fn uninitialized_move;

	///////////////////////////////////////////////////////////////////////////
	// uninitialized_move_n [uninitialized.move]
	//
	template<class I, class O>
	using uninitialized_move_n_result = __in_out_result<I, O>;

	struct __uninitialized_move_n_fn : private __niebloid {
		template<input_iterator I, _NoThrowForwardIterator O, _NoThrowSentinel<O> S>
		requires constructible_from<iter_value_t<O>, iter_rvalue_reference_t<I>>
		uninitialized_move_n_result<I, O>
		operator()(I ifirst, iter_difference_t<I> n, O ofirst, S olast) const {
			auto [in, out] = uninitialized_move(counted_iterator{std::move(ifirst), n},
				default_sentinel, std::move(ofirst), std::move(olast));
			return {in.base(), std::move(out)};
		}
	};

	inline constexpr __uninitialized_move_n_fn uninitialized_move_n;
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_MEMORY_UNINITIALIZED_MOVE_HPP
