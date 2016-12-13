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
#include <type_traits>
#include <utility>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <Range Rng>
		requires
			!(_Is<Rng, std::is_reference> || View<Rng>())
		class ref_view : view_base {
			detail::raw_ptr<Rng> rng_;
		public:
			ref_view() = default;
			constexpr ref_view(Rng& rng) noexcept
			: rng_{__stl2::addressof(rng)} {}

			constexpr iterator_t<Rng> begin() const
			STL2_NOEXCEPT_RETURN(
				__stl2::begin(*rng_)
			)
			constexpr sentinel_t<Rng> end() const
			STL2_NOEXCEPT_RETURN(
				__stl2::end(*rng_)
			)

			constexpr bool empty() const
			STL2_NOEXCEPT_RETURN(
				__stl2::empty(*rng_)
			)

			constexpr auto size() const
			noexcept(noexcept(__stl2::size(*rng_)))
			requires SizedRange<Rng>()
			{ return __stl2::size(*rng_); }

			constexpr auto data() const
			noexcept(noexcept(__stl2::data(*rng_)))
			requires requires { __stl2::data(*rng_); }
			{ return __stl2::data(*rng_); }
		};

		template <Range Rng>
		requires !View<Rng>()
		constexpr ref_view<Rng> as_view(Rng& rng) noexcept
		{ return ref_view<Rng>{rng}; }

		template <class T, View V = __uncvref<T>>
		requires Constructible<V, T>()
		constexpr T&& as_view(T&& t) noexcept
		{ return std::forward<T>(t); }

		template <Range Rng>
		using as_view_t = std::decay_t<decltype(ext::as_view(std::declval<Rng>()))>;
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
