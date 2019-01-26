// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2017
//  Copyright Dalton M. Woodard 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_SINGLE_REF_HPP
#define STL2_VIEW_SINGLE_REF_HPP

#include <memory>
#include <utility>

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template<class T>
		requires std::is_object_v<T>
		class single_ref_view : public view_interface<single_ref_view<T>> {
		private:
			T* ref_ = nullptr;

			// following the same constraints as ref_view
			static void fun(T&) noexcept;
			static void fun(T&&) = delete;
		public:
			constexpr single_ref_view() = default;

#if STL2_WORKAROUND_CLANGC_42
			template<class U>
			requires _NotSameAs<U, single_ref_view> &&
				requires(U&& u) { fun(static_cast<U&&>(u)); }
#else
			template<_NotSameAs<single_ref_view> U>
			requires requires(U&& u) { fun(static_cast<U&&>(u)); }
#endif
			constexpr explicit single_ref_view(U&& u)
			noexcept(noexcept(fun(static_cast<U&&>(u)))) // strengthened
			: ref_{std::addressof(static_cast<T&>(static_cast<U&&>(u)))} {}

			constexpr T* begin() noexcept { return data(); }
			constexpr const T* begin() const noexcept { return data(); }
			constexpr T* end() noexcept { return data() + 1; }
			constexpr const T* end() const noexcept { return data() + 1; }
			constexpr static std::ptrdiff_t size() noexcept { return 1; }
			constexpr T* data() noexcept { return ref_; }
			constexpr const T* data() const noexcept { return ref_; }
		};

		template <class T>
		single_ref_view(T&) -> single_ref_view<T>;
	} // namespace ext

	namespace view::ext {
		struct __single_ref_fn : detail::__pipeable<__single_ref_fn> {
			template<class T>
			constexpr auto operator()(T&& t) const
			STL2_REQUIRES_RETURN(
				__stl2::ext::single_ref_view{std::forward<T>(t)}
			)
		};

		inline constexpr __single_ref_fn single_ref {};
	}
} STL2_CLOSE_NAMESPACE

#endif
