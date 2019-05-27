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
#include <stl2/view/ref.hpp>
#include <stl2/view/subrange.hpp>

STL2_OPEN_NAMESPACE {
	namespace view {
		struct __all_fn : detail::__pipeable<__all_fn> {
		private:
			enum : unsigned { __throws = 1, __decay = 2, __ref = 4, __subrange = 6 };

			template<ViewableRange _Range>
			static constexpr unsigned __choose() noexcept {
				if constexpr (View<__uncvref<_Range>>) {
					return __decay | std::is_nothrow_constructible_v<__uncvref<_Range>, _Range>;
				} else if constexpr (std::is_lvalue_reference_v<_Range>) {
					return __ref | noexcept(ref_view{std::declval<_Range>()});
				} else {
					return __subrange | noexcept(subrange{std::declval<_Range>()});
				}
			}
		public:
			template<ViewableRange _Range, unsigned _Choice = __choose<_Range>()>
			constexpr auto operator()(_Range&& __r) const noexcept(_Choice & __throws) {
				constexpr auto __strategy = _Choice & ~__throws;
				if constexpr (__strategy == __decay) {
					return static_cast<_Range&&>(__r);
				} else if constexpr (__strategy == __ref) {
					return ref_view{__r};
				} else {
					return subrange{static_cast<_Range&&>(__r)};
				}
			}
		};

		inline constexpr __all_fn all {};
	} // namespace view

	template<ViewableRange R>
	using all_view = decltype(view::all(std::declval<R>()));
} STL2_CLOSE_NAMESPACE

#endif
