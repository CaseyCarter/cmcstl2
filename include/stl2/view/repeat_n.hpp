// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015-2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_REPEAT_N_HPP
#define STL2_VIEW_REPEAT_N_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/view/repeat.hpp>
#include <stl2/view/take_exactly.hpp>

STL2_OPEN_NAMESPACE {
	template <Semiregular T>
	using repeat_n_view = take_exactly_view<repeat_view<T>>;
} STL2_CLOSE_NAMESPACE

#endif
