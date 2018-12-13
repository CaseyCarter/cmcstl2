// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_RANGE_NTH_ITERATOR_HPP
#define STL2_DETAIL_RANGE_NTH_ITERATOR_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/operations.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/range/primitives.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		struct __nth_iterator {
			template<Range R>
			constexpr auto operator()(R&& r, iter_difference_t<iterator_t<R>> n) const
			{
				STL2_EXPECT(n >= 0);
				if constexpr (SizedRange<R>) {
					auto const size = distance(r);
					constexpr bool CommonNonRandom = CommonRange<R> && !RandomAccessRange<R>;
					if (n >= size) {
						if constexpr (CommonNonRandom && !BidirectionalRange<R>) {
							// If the range is RandomAccess or Bidirectional, this is just extra codegen
							// with no performance improvement over the following cases.
							return end(r);
						}
						n = size;
					}
					if constexpr (CommonNonRandom && BidirectionalRange<R>) {
						// Again, this would not be an improvement for RandomAccess ranges.
						if (n > size / 2) {
							return prev(end(r), size - n);
						}
					}
					return next(begin(r), n);
				}
				else {
					return next(begin(r), n, end(r));
				}
			}
		};

		inline constexpr __nth_iterator nth_iterator;
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_RANGE_NTH_ITERATOR_HPP
