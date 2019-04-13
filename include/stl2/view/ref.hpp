// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_REF_HPP
#define STL2_VIEW_REF_HPP

#include <memory>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	// ref_view [ranges.view.ref]
	template<Range R>
	requires std::is_object_v<R>
	struct ref_view;

#if STL2_WORKAROUND_CLANG_37556
	namespace __ref_view_detail {
		struct __adl_hook {};

		template<class R>
		constexpr iterator_t<R> begin(ref_view<R> r) {
			return r.begin();
		}
		template<class R>
		constexpr sentinel_t<R> end(ref_view<R> r) {
			return r.end();
		}
	}
#endif // STL2_WORKAROUND_CLANG_37556

	template<Range R>
	requires std::is_object_v<R>
	struct ref_view
	: view_interface<ref_view<R>>
#if STL2_WORKAROUND_CLANG_37556
	, private __ref_view_detail::__adl_hook
#endif // STL2_WORKAROUND_CLANG_37556
	{
	private:
		R* r_ = nullptr;

		static void fun(R&) noexcept; // not defined
		static void fun(R&&) = delete;
	public:
		constexpr ref_view() noexcept = default;

#if STL2_WORKAROUND_CLANGC_42
		template<class T>
		requires _NotSameAs<T, ref_view> &&
			requires(T&& t) { fun(static_cast<T&&>(t)); }
#else // ^^^ workaround / no workaround vvv
		template<_NotSameAs<ref_view> T>
		requires requires(T&& t) { fun(static_cast<T&&>(t)); }
#endif // STL2_WORKAROUND_CLANGC_42
		constexpr ref_view(T&& t)
		noexcept(is_nothrow_convertible_v<T, R&>) // strengthened
		: r_{std::addressof(static_cast<R&>(static_cast<T&&>(t)))} {}

		constexpr R& base() const noexcept { return *r_; }

		constexpr iterator_t<R> begin() const { return __stl2::begin(*r_); }
		constexpr sentinel_t<R> end() const { return __stl2::end(*r_); }

#if !STL2_WORKAROUND_CLANG_37556
		friend constexpr iterator_t<R> begin(ref_view r) {
			return r.begin();
		}
		friend constexpr sentinel_t<R> end(ref_view r) {
			return r.end();
		}
#endif // !STL2_WORKAROUND_CLANG_37556

		constexpr bool empty() const requires detail::CanEmpty<R> {
			return __stl2::empty(*r_);
		}

		constexpr auto size() const requires SizedRange<R> {
			return __stl2::size(*r_);
		}

		constexpr auto data() const requires ContiguousRange<R> {
			return __stl2::data(*r_);
		}
	};

	// This deduction guide is the P/R for LWG 3173
	template<class R>
	ref_view(R&) -> ref_view<R>;

	namespace view::ext {
		struct __ref_fn : detail::__pipeable<__ref_fn> {
			template<class R>
			auto operator()(R&& r) const
#if STL2_WORKAROUND_CLANGC_50
			requires requires(R&& r) { ref_view{std::forward<R>(r)}; } {
				return ref_view{std::forward<R>(r)};
			}
#else // ^^^ workaround / no workaround vvv
			STL2_REQUIRES_RETURN(
				ref_view{std::forward<R>(r)}
			)
#endif // STL2_WORKAROUND_CLANGC_50
		};

		inline constexpr __ref_fn ref {};
	} // namespace view::ext
} STL2_CLOSE_NAMESPACE

#endif
