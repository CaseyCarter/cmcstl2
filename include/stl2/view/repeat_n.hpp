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

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/view/repeat.hpp>
#include <stl2/view/take_exactly.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template<CopyConstructibleObject T>
		using repeat_n_view = take_exactly_view<repeat_view<T>>;
	} // namespace ext

	namespace view::ext {
		struct __repeat_n_fn {
		private:
		template<class T> using V = __stl2::ext::repeat_n_view<__uncvref<T>>;
		public:
			template<class T>
			constexpr auto operator()(T&& t, std::ptrdiff_t const n) const
			noexcept(noexcept(V<T>{repeat(static_cast<T&&>(t)), n}))
#if STL2_WORKAROUND_CLANGC_50
			requires requires(T&& t, std::ptrdiff_t n) {
				V<T>{repeat(static_cast<T&&>(t)), n};
			} {
#else // ^^^ workaround / no workaround vvv
			requires requires { V<T>{repeat(static_cast<T&&>(t)), n}; } {
#endif // STL2_WORKAROUND_CLANGC_50
				STL2_EXPECT(n >= 0);
				return V<T>{repeat(static_cast<T&&>(t)), n};
			}
		};

		inline constexpr __repeat_n_fn repeat_n {};
	} // namespace view::ext
} STL2_CLOSE_NAMESPACE

#endif
