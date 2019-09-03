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
		template<copy_constructible_object T>
		using repeat_n_view = take_exactly_view<repeat_view<T>>;
	} // namespace ext

	namespace views::ext {
		struct __repeat_n_fn {
		private:
		template<class T> using V = __stl2::ext::repeat_n_view<__uncvref<T>>;
		public:
			template<class T>
			constexpr auto operator()(T&& t, std::ptrdiff_t const n) const
			noexcept(noexcept(V<T>{repeat(static_cast<T&&>(t)), n}))
			requires requires { V<T>{repeat(static_cast<T&&>(t)), n}; } {
				STL2_EXPECT(n >= 0);
				return V<T>{repeat(static_cast<T&&>(t)), n};
			}
		};

		inline constexpr __repeat_n_fn repeat_n {};
	} // namespace views::ext
} STL2_CLOSE_NAMESPACE

#endif
