// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_COUNTED_VIEW_HPP
#define STL2_VIEW_COUNTED_VIEW_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/view/subrange.hpp>

STL2_OPEN_NAMESPACE {
	namespace view {
		struct __counted_fn {
			template<Iterator I>
			constexpr auto operator()(I i, iter_difference_t<I> d) const {
				if constexpr (RandomAccessIterator<I>) {
					return subrange{i, i + d};
				} else {
					return subrange{counted_iterator{i, d}, default_sentinel{}};
				}
			}
		};

		inline constexpr __counted_fn counted {};
	}
} STL2_CLOSE_NAMESPACE

#endif
