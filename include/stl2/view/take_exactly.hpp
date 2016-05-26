// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_VIEW_TAKE_EXACTLY_HPP
#define STL2_DETAIL_VIEW_TAKE_EXACTLY_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/detail/range/concepts.hpp>

STL2_OPEN_NAMESPACE {
	template <View Base>
	class take_exactly_view : Base {
		using iterator = counted_iterator<iterator_t<Base>>;
		std::ptrdiff_t n_ = 0;
	public:
		take_exactly_view() = default;
		constexpr take_exactly_view(Base view, std::ptrdiff_t n)
		noexcept(is_nothrow_move_constructible<Base>::value)
		: Base{__stl2::move(view)}, n_{n}
		{ STL2_ASSUME_CONSTEXPR(n >= 0); }

		constexpr iterator begin() const
		noexcept(noexcept(iterator{__stl2::begin(declval<Base const&>()), n_}))
		{ return {__stl2::begin(static_cast<Base const&>(*this)), n_}; }
		constexpr default_sentinel end() const noexcept { return {}; }
		constexpr std::ptrdiff_t size() const noexcept { return n_; }
	};
} STL2_CLOSE_NAMESPACE

#endif
