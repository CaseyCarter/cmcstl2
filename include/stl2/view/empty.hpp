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
#ifndef STL2_VIEW_EMPTY_HPP
#define STL2_VIEW_EMPTY_HPP

#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	template<class T>
	requires std::is_object_v<T>
	struct empty_view : view_interface<empty_view<T>> {
		static constexpr T* begin() noexcept { return nullptr; }
		static constexpr T* end() noexcept { return nullptr; }
		static constexpr T* data() noexcept { return nullptr; }
		static constexpr std::ptrdiff_t size() noexcept { return 0; }
		static constexpr bool empty() noexcept { return true; }

		friend constexpr T* begin(empty_view) noexcept { return nullptr; }
		friend constexpr T* end(empty_view) noexcept { return nullptr; }
	};

	namespace views {
		template<class T>
		inline constexpr empty_view<T> empty;
	}
} STL2_CLOSE_NAMESPACE

#endif
