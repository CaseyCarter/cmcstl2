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

#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/algorithm/return_types.hpp>

///////////////////////////////////////////////////////////////////////////
// move_backward [alg.move]
//
STL2_OPEN_NAMESPACE {
	template <class I1, class I2>
	struct move_backward_result : __in_out_result<I1, I2> {};

	class __move_backward_fn {
		template<BidirectionalIterator I1, BidirectionalIterator I2>
		requires IndirectlyMovable<I1, I2>
		static constexpr move_backward_result<I1, I2> impl(I1 first, I1 last, I2 result)
		{
			auto i = last;
			while (i != first) {
				*--result = __stl2::iter_move(--i);
			}
			return {std::move(last), std::move(result)};
		}

		template<BidirectionalIterator I1, Sentinel<I1> S1, BidirectionalIterator I2>
		requires IndirectlyMovable<I1, I2>
		static constexpr move_backward_result<I1, I2> impl(I1 first, S1 last, I2 result)
		{
			auto end = __stl2::next(first, std::move(last));
			return __move_backward_fn::impl(
				std::move(first), std::move(end), std::move(result));
		}
	public:
		template<BidirectionalIterator I1, Sentinel<I1> S1, BidirectionalIterator I2>
		requires IndirectlyMovable<I1, I2>
		constexpr move_backward_result<I1, I2> operator()(I1 first, S1 last, I2 result) const
		{
			return __move_backward_fn::impl(std::move(first), std::move(last), std::move(result));
		}

		template<BidirectionalRange Rng, BidirectionalIterator I>
		requires IndirectlyMovable<iterator_t<Rng>, I>
		constexpr move_backward_result<safe_iterator_t<Rng>, I> operator()(Rng&& rng, I result) const
		{
			return __move_backward_fn::impl(__stl2::begin(rng), __stl2::end(rng), std::move(result));
		}
	};

	inline constexpr __move_backward_fn move_backward {};
} STL2_CLOSE_NAMESPACE

#endif
