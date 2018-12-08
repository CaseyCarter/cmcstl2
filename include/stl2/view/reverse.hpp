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
#ifndef STL2_VIEW_REVERSE_HPP
#define STL2_VIEW_REVERSE_HPP

#include <stl2/detail/cached_position.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/reverse_iterator.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>
#include <stl2/detail/view/view_closure.hpp>

STL2_OPEN_NAMESPACE {
	template<View Rng>
	requires BidirectionalRange<Rng>
	class reverse_view
	: public view_interface<reverse_view<Rng>>
	, private detail::cached_position<Rng, reverse_view<Rng>, !CommonRange<Rng>> {
		using __iterator = reverse_iterator<iterator_t<Rng>>;

		Rng base_;

		constexpr auto& end_() noexcept
		{ return static_cast<typename reverse_view::cached_position&>(*this); }
	public:
		reverse_view() = default;

		constexpr explicit reverse_view(Rng rng)
		: base_{std::move(rng)} {}

		template<ViewableRange O>
		requires BidirectionalRange<O> && _ConstructibleFromRange<Rng, O>
		constexpr explicit reverse_view(O&& o)
		: base_(view::all(std::forward<O>(o))) {}

		constexpr Rng base() const
		{ return base_; }

		constexpr __iterator begin()
		{
			const auto cached = static_cast<bool>(end_());
			iterator_t<Rng> first = cached
				? end_().get(base_)
				: __stl2::next(__stl2::begin(base_), __stl2::end(base_));
			if (!cached)
				end_().set(base_, first);
			return __iterator{std::move(first)};
		}

		constexpr __iterator begin() requires CommonRange<Rng>
		{ return __iterator{__stl2::end(base_)}; }

		constexpr __iterator begin() const requires CommonRange<const Rng>
		{ return __iterator{__stl2::end(base_)}; }

		constexpr __iterator end()
		{ return __iterator{__stl2::begin(base_)}; }

		constexpr __iterator end() const requires CommonRange<const Rng>
		{ return __iterator{__stl2::begin(base_)}; }

		constexpr auto size() const requires SizedRange<const Rng>
		{ return __stl2::size(base_); }
	};

	template<class Rng>
	reverse_view(Rng&&) -> reverse_view<all_view<Rng>>;

	namespace view {
		struct __reverse_fn : detail::__pipeable<__reverse_fn> {
			template<BidirectionalRange Rng>
			requires ViewableRange<Rng>
			constexpr auto operator()(Rng&& rng) const {
				return reverse_view{std::forward<Rng>(rng)};
			}
		};

		inline constexpr __reverse_fn reverse {};
	} // namespace view
} STL2_CLOSE_NAMESPACE

#endif
