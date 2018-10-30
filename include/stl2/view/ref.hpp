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

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/memory/addressof.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/view/view_interface.hpp>
#include <stl2/detail/view/view_closure.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template<Range R>
		requires _Is<R, std::is_object>
		struct ref_view : view_interface<ref_view<R>> {
		private:
			R* r_ = nullptr;

			static void fun(R&) noexcept; // not defined
			static void fun(R&&) = delete;

		public:
			constexpr ref_view() noexcept = default;

			template<_NotSameAs<ref_view> T>
			requires requires(T&& t) { fun(static_cast<T&&>(t)); }
			constexpr ref_view(T&& t)
#if 0
			noexcept(std::is_nothrow_convertible_v<T, R&>)
#else
			noexcept(noexcept(fun(static_cast<T&&>(t))))
#endif
			: r_{detail::addressof(static_cast<R&>(static_cast<T&&>(t)))} {}

			constexpr R& base() const noexcept
			{ return *r_; }

			constexpr iterator_t<R> begin() const
			{ return __stl2::begin(*r_); }

			constexpr sentinel_t<R> end() const
			{ return __stl2::end(*r_); }

			constexpr bool empty() const
			requires detail::CanEmpty<R>
			{ return __stl2::empty(*r_); }

			constexpr auto size() const
			requires SizedRange<R>
			{ return __stl2::size(*r_); }

			constexpr auto data() const
			requires ContiguousRange<R>
			{ return __stl2::data(*r_); }
		};

		template<class R>
		ref_view(R&) -> ref_view<R>;

		template<class R>
		constexpr iterator_t<R> begin(ref_view<R> r)
		{ return r.begin(); }

		template<class R>
		constexpr sentinel_t<R> end(ref_view<R> r)
		{ return r.end(); }
	} // namespace ext

	namespace view::ext {
		struct __ref_fn : detail::__pipeable<__ref_fn> {
			template<class R>
			auto operator()(R&& rng) const
			STL2_NOEXCEPT_REQUIRES_RETURN(
				__stl2::ext::ref_view(std::forward<R>(rng))
			)
		};

		inline constexpr __ref_fn ref {};
	} // namespace view::ext
} STL2_CLOSE_NAMESPACE

#endif
