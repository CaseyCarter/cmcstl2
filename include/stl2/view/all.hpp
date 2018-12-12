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
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/ref.hpp>
#include <stl2/view/subrange.hpp>

STL2_OPEN_NAMESPACE {
	namespace view {
		struct __all_fn : detail::__pipeable<__all_fn> {
			template<Range R>
			requires View<__f<R>>
			constexpr auto operator()(R&& r) const
			noexcept(std::is_nothrow_constructible_v<__f<R>, R>) {
				return static_cast<R&&>(r);
			}

			template<_ForwardingRange R>
			requires (!View<__uncvref<R>>)
			constexpr auto operator()(R&& r) const
			noexcept(std::is_reference_v<R>)
			{
				if constexpr (std::is_reference_v<R>) {
					return ref_view{r};
				} else {
					return subrange{static_cast<R&&>(r)};
				}
			}
		};

		inline constexpr __all_fn all {};
	} // namespace view

	template<ViewableRange R>
	using all_view = decltype(view::all(std::declval<R>()));
} STL2_CLOSE_NAMESPACE

#endif
