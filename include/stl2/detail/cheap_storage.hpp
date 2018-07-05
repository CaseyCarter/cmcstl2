// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_CHEAP_STORAGE_HPP
#define STL2_DETAIL_CHEAP_STORAGE_HPP

#include <stl2/detail/ebo_box.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>

STL2_OPEN_NAMESPACE {
	namespace detail {
		constexpr std::size_t cheap_copy_size = 32;

		template <class>
		constexpr bool cheaply_copyable = false;
		template <class T>
		requires
			CopyConstructible<T> &&
			std::is_trivially_copyable<T>::value &&
			((_Is<T, std::is_empty> && !_Is<T, std::is_final>) ||
				sizeof(T) <= cheap_copy_size)
		constexpr bool cheaply_copyable<T> = true;

		template <_Is<std::is_object> T, class Tag = void>
		class ref_box {
		public:
			ref_box() = default;
			constexpr ref_box(T& t) noexcept
			: ptr_{&t} {}
			ref_box(T&&) = delete;

			constexpr T& get() const noexcept {
				return *ptr_;
			}

		private:
			raw_ptr<T> ptr_;
		};

		// Note: promotes to CopyConstructible
		template <_Is<std::is_object> T, class Tag = void>
		using cheap_reference_box_t = meta::if_c<
			cheaply_copyable<remove_cv_t<T>>,
			ebo_box<remove_cv_t<T>, Tag>,
			ref_box<T, Tag>>;
	}
} STL2_CLOSE_NAMESPACE

#endif
