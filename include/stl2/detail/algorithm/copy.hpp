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
#ifndef STL2_DETAIL_ALGORITHM_COPY_HPP
#define STL2_DETAIL_ALGORITHM_COPY_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/return_types.hpp>

///////////////////////////////////////////////////////////////////////////
// copy [alg.copy]
//
STL2_OPEN_NAMESPACE {
	template <class I, class O>
	struct copy_result : __in_out_result<I, O> {};

	struct __copy_fn {
		template<InputIterator I, Sentinel<I> S, WeaklyIncrementable O>
		requires IndirectlyCopyable<I, O>
		constexpr copy_result<I, O> operator()(I first, S last, O result) const
		{
			for (; first != last; ++first, ++result) {
				*result = *first;
			}
			return {std::move(first), std::move(result)};
		}

		template<InputRange Rng, WeaklyIncrementable O>
		requires IndirectlyCopyable<iterator_t<Rng>, O>
		constexpr copy_result<safe_iterator_t<Rng>, O> operator()(Rng&& rng, O result) const
		{
			return (*this)(__stl2::begin(rng), __stl2::end(rng), std::forward<O>(result));
		}
	};

	inline constexpr __copy_fn copy {};

	namespace ext {
		struct __copy_ext_fn {
			template <InputIterator I1, Sentinel<I1> S1, Iterator I2, Sentinel<I2> S2>
			requires
				IndirectlyCopyable<I1, I2>
			constexpr copy_result<I1, I2>
			operator()(I1 first, S1 last, I2 result_first, S2 result_last) const
			{
				for (; first != last && result_first != result_last; ++first, ++result_first) {
					*result_first = *first;
				}
				return {std::move(first), std::move(result_first)};
			}

			template <InputRange Rng1, Range Rng2>
			requires
				IndirectlyCopyable<iterator_t<Rng1>, iterator_t<Rng2>>
			constexpr copy_result<safe_iterator_t<Rng1>, safe_iterator_t<Rng2>>
			operator()(Rng1&& rng1, Rng2&& rng2) const
			{
				return (*this)(__stl2::begin(rng1), __stl2::end(rng1),
					__stl2::begin(rng2), __stl2::end(rng2));
			}
		};
		
		inline constexpr __copy_ext_fn copy {};
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
