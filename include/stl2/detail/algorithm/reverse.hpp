// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
// Copyright (c) 2009 Alexander Stepanov and Paul McJones
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without
// fee, provided that the above copyright notice appear in all copies
// and that both that copyright notice and this permission notice
// appear in supporting documentation. The authors make no
// representations about the suitability of this software for any
// purpose. It is provided "as is" without express or implied
// warranty.
//
// Algorithms from
// Elements of Programming
// by Alexander Stepanov and Paul McJones
// Addison-Wesley Professional, 2009
//
#ifndef STL2_DETAIL_ALGORITHM_REVERSE_HPP
#define STL2_DETAIL_ALGORITHM_REVERSE_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/tagged.hpp>
#include <stl2/detail/temporary_vector.hpp>
#include <stl2/detail/algorithm/move.hpp>
#include <stl2/detail/algorithm/swap_ranges.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// reverse [alg.reverse]
//
STL2_OPEN_NAMESPACE {
	class __reverse_fn {
		template <BidirectionalIterator I>
		static constexpr I impl(I first, I last)
		{
			auto m = last;
			while (first != m && first != --m) {
				__stl2::iter_swap(first, m);
				++first;
			}
			return last;
		}

		template <RandomAccessIterator I>
		static constexpr I impl(I first, I last)
		{
			if (first != last) {
				auto m = last;
				while (first < --m) {
					__stl2::iter_swap(first, m);
					++first;
				}
			}
			return last;
		}
	public:
		template<BidirectionalIterator I, Sentinel<I> S>
		requires Permutable<I>
		constexpr I operator()(I first, S last) const
		{
			auto bound = __stl2::next(first, std::move(last));
			return __reverse_fn::impl(std::move(first), std::move(bound));
		}

		template<BidirectionalRange Rng>
		requires Permutable<iterator_t<Rng>>
		constexpr safe_iterator_t<Rng> operator()(Rng&& rng) const
		{
			return (*this)(__stl2::begin(rng), __stl2::end(rng));
		}
	};

	inline constexpr __reverse_fn reverse {};

	namespace ext {
		class __reverse_fn {
			// An adaptation of the EoP algorithm reverse_n_with_buffer
			// Complexity: n moves + n / 2 swaps
			template <class I>
			requires
				Permutable<I>
			static constexpr I reverse_n_with_half_buffer(I first, const iter_difference_t<I> n,
				__stl2::detail::temporary_buffer<iter_value_t<I>>& buf)
			{
				// Precondition: $\property{mutable\_counted\_range}(first, n)$
				STL2_EXPECT(n / 2 <= buf.size());

				auto&& vec = __stl2::detail::make_temporary_vector(buf);
				auto ufirst = __stl2::ext::uncounted(first);
				// Shift the first half of the input range into the buffer.
				auto umiddle = __stl2::ext::uncounted(__stl2::move(
					__stl2::make_counted_iterator(ufirst, n / 2),
					default_sentinel{},
					__stl2::back_inserter(vec)).in);
				if (n % 2 != 0) {
					++umiddle;
				}
				// Swap the buffered elements in reverse order with the second half of
				// the input range.
				auto ulast = __stl2::swap_ranges(__stl2::rbegin(vec), __stl2::rend(vec),
					std::move(umiddle), unreachable{}).in2;
				// Shift the buffer contents into the first half of the input range.
				__stl2::move(vec, std::move(ufirst));
				return __stl2::ext::recounted(first, std::move(ulast), n);
			}

			// From EoP
			template <class I>
			requires
				Permutable<I>
			static constexpr I reverse_n_adaptive(I first, const iter_difference_t<I> n,
				__stl2::detail::temporary_buffer<iter_value_t<I>>& buf)
			{
				// Precondition: $\property{mutable\_counted\_range}(first, n)$
				if (n < iter_difference_t<I>(2)) {
					return __stl2::next(std::move(first), n);
				}

				const auto half_n = n / 2;
				if (half_n <= buf.size()) {
					return __reverse_fn::reverse_n_with_half_buffer(std::move(first), n, buf);
				}

				auto last1 = __reverse_fn::reverse_n_adaptive(first, half_n, buf);
				auto first2 = last1;
				if (2 * half_n != n) {
					++first2;
				}
				__reverse_fn::reverse_n_adaptive(first2, half_n, buf);
				return __stl2::swap_ranges(std::move(first), std::move(last1),
					std::move(first2), unreachable{}).in2;
			}

			template <class I>
			requires
				Permutable<I>
			static constexpr I reverse_n(I first, iter_difference_t<I> n)
			{
				auto ufirst = ext::uncounted(first);
				using buf_t = __stl2::detail::temporary_buffer<iter_value_t<decltype(ufirst)>>;
				// TODO: tune this threshold.
				constexpr auto alloc_threshold = iter_difference_t<I>(8);
				auto buf = n >= alloc_threshold ? buf_t{n / 2} : buf_t{};
				auto last = __reverse_fn::reverse_n_adaptive(ufirst, n, buf);
				return ext::recounted(first, std::move(last), n);
			}
		public:
			// Extension
			template <Permutable I, Sentinel<I> S>
			constexpr I operator()(I first, S last) const
			{
				auto n = __stl2::distance(first, std::move(last));
				return __reverse_fn::reverse_n(std::move(first), n);
			}

			// Extension
			template <ForwardRange Rng>
			requires Permutable<iterator_t<Rng>>
			constexpr safe_iterator_t<Rng> operator()(Rng&& rng) const
			{
				return __reverse_fn::reverse_n(__stl2::begin(rng), __stl2::distance(rng));
			}
		};

		inline constexpr __reverse_fn reverse {};
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
