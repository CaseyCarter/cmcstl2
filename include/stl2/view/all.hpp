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
#ifndef STL2_VIEW_ALL_HPP
#define STL2_VIEW_ALL_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/subrange.hpp>
#include <stl2/view/ref.hpp>

STL2_OPEN_NAMESPACE {
	namespace view {
		struct __all_fn : detail::__pipeable<__all_fn> {
			template <Range Rng>
			requires View<__f<Rng>>
			constexpr auto operator()(Rng&& rng) const
			noexcept(std::is_nothrow_constructible_v<__f<Rng>, Rng>)
			{ return std::forward<Rng>(rng); }

			template <_ForwardingRange Rng>
			requires !View<__uncvref<Rng>>
			constexpr auto operator()(Rng&& rng) const
			noexcept(std::is_reference_v<Rng>)
			{
				if constexpr (std::is_reference_v<Rng>) {
					return __stl2::ext::ref_view{rng};
				} else {
					return subrange{static_cast<Rng&&>(rng)};
				}
			}
		};

		inline constexpr __all_fn all {};
	} // namespace view

	template <ViewableRange Rng>
	using all_view = decltype(view::all(std::declval<Rng>()));

	// Work-around for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
	template <class V, class R>
	concept bool _ConstructibleFromRange =
		requires { typename all_view<R>; } &&
		View<V> && Constructible<V, all_view<R>>;
} STL2_CLOSE_NAMESPACE

#endif
