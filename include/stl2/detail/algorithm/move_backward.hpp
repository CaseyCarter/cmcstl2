// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_MOVE_BACKWARD_HPP
#define STL2_DETAIL_ALGORITHM_MOVE_BACKWARD_HPP

#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// move_backward [alg.move]
//
STL2_OPEN_NAMESPACE {
	template<class I1, class I2>
	using move_backward_result = __in_out_result<I1, I2>;

	struct __move_backward_fn : private __niebloid {
		template<bidirectional_iterator I1, sentinel_for<I1> S1,
			bidirectional_iterator I2>
		requires indirectly_movable<I1, I2>
		constexpr move_backward_result<I1, I2>
		operator()(I1 first, S1 s, I2 result) const {
			auto last = next(first, std::move(s));
			auto i = last;
			while (i != first) {
				*--result = iter_move(--i);
			}
			return {std::move(last), std::move(result)};
		}

		template<bidirectional_range Rng, bidirectional_iterator I>
		requires indirectly_movable<iterator_t<Rng>, I>
		constexpr move_backward_result<safe_iterator_t<Rng>, I>
		operator()(Rng&& rng, I result) const {
			return (*this)(begin(rng), end(rng), std::move(result));
		}
	};

	inline constexpr __move_backward_fn move_backward{};
} STL2_CLOSE_NAMESPACE

#endif
