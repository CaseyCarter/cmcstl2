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

STL2_OPEN_NAMESPACE {
	namespace view {
		namespace __all {
			struct fn : detail::__pipeable<fn> {
				template <Range Rng>
				requires View<__f<Rng>>
				constexpr __f<Rng> operator()(Rng&& rng) const
				{ return std::forward<Rng>(rng); }

				template <Range Rng>
				requires std::is_lvalue_reference_v<Rng> && !View<std::decay_t<Rng>>
				constexpr auto operator()(Rng&& rng) const
				{ return ext::subrange{rng}; }
			};
		}

		inline constexpr __all::fn all {};
	}

	namespace ext {
		template <ViewableRange Rng>
		using all_view = decltype(view::all(std::declval<Rng>()));
	}

	// Work-around for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
	template <class V, class R>
	concept bool _ConstructibleFromRange =
		View<V> && ext::ViewableRange<R> && Constructible<V, ext::all_view<R>>;
} STL2_CLOSE_NAMESPACE

#endif
