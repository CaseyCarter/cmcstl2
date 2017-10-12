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

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/view/view_interface.hpp>
#include <stl2/detail/semiregular_box.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <CopyConstructible T>
		class single_view : view_interface<single_view<T>> {
		private:
			detail::semiregular_box<T> value_;
		public:
			single_view() = default;
			constexpr explicit single_view(const T& t)
			: value_(t) {}
			constexpr explicit single_view(T&& t)
			: value_(std::move(t)) {}

			template <class... Args>
			requires Constructible<T, Args...>
			constexpr single_view(in_place_t, Args&&... args)
			: value_(in_place, std::forward<Args>(args)...) {}

			using iterator = const T*;
			using const_iterator = const T*;
			using sentinel = const T*;
			using const_sentinel = const T*;

			constexpr const T* begin() const noexcept
			{ return std::addressof(value_.get()); }
			constexpr const T* end() const noexcept
			{ return begin() + 1; }
			constexpr static ptrdiff_t size() noexcept
			{ return 1; }
			constexpr const T* data() const noexcept
			{ return begin(); }
		};

		template <class T>
		requires CopyConstructible<__f<T>> && !meta::is<__f<T>, single_view>::value
		explicit single_view(T&&) -> single_view<std::decay_t<T>>;
	}

	namespace view {
		namespace __single {
			struct fn {
				template <class T>
				requires CopyConstructible<__f<T>>
				constexpr auto operator()(T&& t) const
				{ return ext::single_view{std::forward<T>(t)}; }
			};
		}

		inline constexpr __single::fn single {};
	}
} STL2_CLOSE_NAMESPACE

#endif
