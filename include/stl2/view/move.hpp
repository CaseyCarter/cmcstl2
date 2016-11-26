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
#ifndef STL2_DETAIL_VIEW_MOVE_HPP
#define STL2_DETAIL_VIEW_MOVE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/move_iterator.hpp>
#include <stl2/detail/range/concepts.hpp>

STL2_OPEN_NAMESPACE {
	template <InputRange Rng>
	class move_view {
		Rng rng_;
	public:
		move_view() requires DefaultConstructible<Rng>() = default;
		template <class... Args>
		move_view(Args&&... args)
		requires Constructible<Rng, Args...>()
		: rng_{static_cast<Args&&>(args)...} {}

		move_iterator<iterator_t<Rng>> begin() const
		requires Range<const Rng>()
		{
			return __stl2::make_move_iterator(__stl2::begin(rng_));
		}
		move_sentinel<sentinel_t<Rng>> end() const
		requires Range<const Rng>()
		{
			return __stl2::make_move_sentinel(__stl2::end(rng_));
		}
		move_iterator<iterator_t<Rng>> end() const
		requires Range<const Rng>() && BoundedRange<Rng>()
		{
			return __stl2::make_move_iterator(__stl2::end(rng_));
		}
		auto size() const
		requires SizedRange<const Rng>()
		{
			return __stl2::size(rng_);
		}
		bool empty() const
		requires
			requires(const Rng& r) { __stl2::empty(r); }
		{
			return __stl2::empty(rng_);
		}

		move_iterator<iterator_t<Rng>> begin()
		{
			return __stl2::make_move_iterator(__stl2::begin(rng_));
		}
		move_sentinel<sentinel_t<Rng>> end()
		{
			return __stl2::make_move_sentinel(__stl2::end(rng_));
		}
		move_iterator<iterator_t<Rng>> end()
		requires BoundedRange<Rng>()
		{
			return __stl2::make_move_iterator(__stl2::end(rng_));
		}
		auto size()
		requires SizedRange<Rng>()
		{
			return __stl2::size(rng_);
		}
		bool empty()
		{
			return __stl2::empty(rng_);
		}
	};
} STL2_CLOSE_NAMESPACE

#endif
