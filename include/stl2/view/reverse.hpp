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

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/reverse_iterator.hpp>
#include <stl2/detail/non_propagating_cache.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>
#include <stl2/detail/view/view_closure.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <View Rng>
		requires BidirectionalRange<Rng>
		class reverse_view
		: public view_interface<reverse_view<Rng>>
		, private detail::non_propagating_cache<iterator_t<Rng>, void, !models::BoundedRange<Rng>> {
			Rng base_;
			constexpr auto& end_() noexcept
			{ return static_cast<typename reverse_view::non_propagating_cache&>(*this); }
		public:
			reverse_view() = default;

			constexpr explicit reverse_view(Rng rng)
			: base_{std::move(rng)} {}

			template <ViewableRange O>
			requires BidirectionalRange<O> && _ConstructibleFromRange<Rng, O>
			constexpr explicit reverse_view(O&& o)
			: base_(view::all(std::forward<O>(o))) {}

			constexpr Rng base() const
			{ return base_; }

			constexpr auto begin()
			{
				if (!end_())
					end_() = __stl2::next(__stl2::begin(base_), __stl2::end(base_));
				return __stl2::make_reverse_iterator(*end_());
			}

			constexpr auto begin() requires BoundedRange<Rng>
			{ return __stl2::make_reverse_iterator(__stl2::end(base_)); }

			constexpr auto begin() const requires BoundedRange<const Rng>
			{ return __stl2::make_reverse_iterator(__stl2::end(base_)); }

			constexpr auto end()
			{ return __stl2::make_reverse_iterator(__stl2::begin(base_)); }

			constexpr auto end() const requires BoundedRange<const Rng>
			{ return __stl2::make_reverse_iterator(__stl2::begin(base_)); }

			constexpr auto size() const requires SizedRange<const Rng>
			{ return __stl2::size(base_); }
		};

		template <class Rng>
		reverse_view(Rng&&) -> reverse_view<all_view<Rng>>;
	} // namespace ext

	namespace __reverse {
		struct fn : detail::__pipeable<fn> {
			template <BidirectionalRange Rng>
			requires ext::ViewableRange<Rng>
			constexpr auto operator()(Rng&& rng) const {
				return ext::reverse_view{std::forward<Rng>(rng)};
			}
		};
	} // namespace __reverse

	namespace view {
		inline constexpr __reverse::fn reverse {};
	}
} STL2_CLOSE_NAMESPACE

#endif
