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
#ifndef STL2_DETAIL_ALGORITHM_MOVE_HPP
#define STL2_DETAIL_ALGORITHM_MOVE_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/copy.hpp>
#include <stl2/detail/algorithm/return_types.hpp>

///////////////////////////////////////////////////////////////////////////
// move [alg.move]
//
STL2_OPEN_NAMESPACE {
	template <class I, class O>
	struct move_result : __in_out_result<I, O> {};

	struct __move_fn {
		template<InputIterator I, Sentinel<I> S, WeaklyIncrementable O>
		requires IndirectlyMovable<I, O>
		constexpr move_result<I, O> operator()(I first, S last, O result) const
		{
				for (; first != last; ++first, ++result) {
					*result = __stl2::iter_move(first);
				}
				return {std::move(first), std::move(result)};
		}

		template<InputRange Rng, WeaklyIncrementable O>
		requires IndirectlyMovable<iterator_t<Rng>, O>
		constexpr move_result<safe_iterator_t<Rng>, O> operator()(Rng&& rng, O result) const
		{
			return (*this)(__stl2::begin(rng), __stl2::end(rng), std::forward<O>(result));
		}
	};

	inline constexpr __move_fn move {};

	namespace ext {
		struct __move_ext_fn {
			// Extension
			template <InputIterator I1, Sentinel<I1> S1, Iterator I2, Sentinel<I2> S2>
			requires IndirectlyMovable<I1, I2>
			constexpr move_result<I1, I2> operator()(I1 first1, S1 last1, I2 first2, S2 last2) const
			{
				for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
					*first2 = __stl2::iter_move(first1);
				}
				return {std::move(first1), std::move(first2)};
			}

			// Extension
			template <InputRange Rng1, Range Rng2>
			requires IndirectlyMovable<iterator_t<Rng1>, iterator_t<Rng2>>
			move_result<safe_iterator_t<Rng1>, safe_iterator_t<Rng2>>
			operator()(Rng1&& rng1, Rng2&& rng2) const
			{
				return (*this)(__stl2::begin(rng1), __stl2::end(rng1),
					__stl2::begin(rng2), __stl2::end(rng2));
			}
		};

		// Extension
		inline constexpr __move_ext_fn move {};
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
