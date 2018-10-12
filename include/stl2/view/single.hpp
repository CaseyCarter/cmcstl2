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
#ifndef STL2_VIEW_SINGLE_HPP
#define STL2_VIEW_SINGLE_HPP

#include <memory>
#include <type_traits>
#include <utility>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	template<CopyConstructible T>
	requires std::is_object_v<T>
	class single_view : public view_interface<single_view<T>> {
	private:
		detail::semiregular_box<T> value_;
	public:
		single_view() = default;
		constexpr explicit single_view(const T& t)
		: value_(t) {}
		constexpr explicit single_view(T&& t)
		: value_(std::move(t)) {}

		template<class... Args>
		requires Constructible<T, Args...>
		constexpr single_view(std::in_place_t, Args&&... args)
		: value_(std::in_place, std::forward<Args>(args)...) {}

		constexpr T* begin() noexcept { return data(); }
		constexpr const T* begin() const noexcept { return data(); }
		constexpr T* end() noexcept { return data() + 1; }
		constexpr const T* end() const noexcept { return data() + 1; }
		constexpr static std::ptrdiff_t size() noexcept { return 1; }
		constexpr T* data() noexcept { return std::addressof(value_.get()); }
		constexpr const T* data() const noexcept { return std::addressof(value_.get()); }
	};

	namespace view {
		struct __single_fn {
			template<class T>
			constexpr auto operator()(T&& t) const
			STL2_NOEXCEPT_REQUIRES_RETURN(
				single_view{std::forward<T>(t)}
			)
		};

		inline constexpr __single_fn single {};
	}
} STL2_CLOSE_NAMESPACE

#endif
