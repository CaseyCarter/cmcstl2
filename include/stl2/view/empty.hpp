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

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <class T>
		requires ext::Addressable<T>
		class empty_view : view_interface<empty_view<T>> {
		public:
			empty_view() = default;
			using iterator = T*;
			using const_iterator = T*;
			using sentinel = T*;
			using const_sentinel = T*;
			constexpr static T* begin() noexcept
			{ return nullptr; }
			constexpr static T* end() noexcept
			{ return nullptr; }
			constexpr static ptrdiff_t size() noexcept
			{ return 0; }
			constexpr static T* data() noexcept
			{ return nullptr; }
		};
	}

	namespace view {
		template <class T>
		inline constexpr ext::empty_view<T> empty {};
	}
} STL2_CLOSE_NAMESPACE

#endif
