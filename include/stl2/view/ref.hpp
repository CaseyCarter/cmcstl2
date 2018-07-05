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

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <Range Rng>
		requires _Is<Rng, std::is_object>
		struct ref_view : view_interface<ref_view<Rng>> {
		private:
			Rng* rng_ = nullptr;
		public:
			constexpr ref_view() noexcept = default;
			constexpr ref_view(Rng& rng) noexcept
			: rng_{detail::addressof(rng)} {}

			constexpr Rng& base() const
			STL2_NOEXCEPT_RETURN(
				*rng_
			)

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

		template <class Rng>
		constexpr iterator_t<Rng> begin(ref_view<Rng> r)
		STL2_NOEXCEPT_RETURN(
			r.begin()
		)
		template <class Rng>
		constexpr sentinel_t<Rng> end(ref_view<Rng> r)
		STL2_NOEXCEPT_RETURN(
			r.end()
		)
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
