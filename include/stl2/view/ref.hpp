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

#include <type_traits>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/memory/addressof.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <Range Rng>
		requires !(_Is<Rng, std::is_reference> || View<Rng>)
		class ref_view : public view_interface<ref_view<Rng>> {
			Rng* rng_ = nullptr;
		public:
			constexpr ref_view() noexcept = default;
			constexpr ref_view(Rng& rng) noexcept
			: rng_{detail::addressof(rng)} {}

			constexpr iterator_t<Rng> begin() const
			STL2_NOEXCEPT_RETURN(
				__stl2::begin(*rng_)
			)
			constexpr sentinel_t<Rng> end() const
			STL2_NOEXCEPT_RETURN(
				__stl2::end(*rng_)
			)

			constexpr bool empty() const
			noexcept(noexcept(__stl2::empty(*rng_)))
			requires detail::CanEmpty<Rng>
			{ return __stl2::empty(*rng_); }

			constexpr auto size() const
			noexcept(noexcept(__stl2::size(*rng_)))
			requires SizedRange<Rng>
			{ return __stl2::size(*rng_); }

			constexpr auto data() const
			noexcept(noexcept(__stl2::data(*rng_)))
			requires ContiguousRange<Rng>
			{ return __stl2::data(*rng_); }
		};
	} // namespace ext

	namespace detail {
		template <class Rng>
		inline constexpr bool is_referenceable_range<ext::ref_view<Rng>> = true;
	} // namespace detail
} STL2_CLOSE_NAMESPACE

#endif
