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
#ifndef STL2_VIEW_MOVE_HPP
#define STL2_VIEW_MOVE_HPP

#include <stl2/detail/ebo_box.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/move_iterator.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>
#include <stl2/detail/view/view_closure.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <View Rng>
		requires InputRange<Rng>
		class move_view : detail::ebo_box<Rng, move_view<Rng>>
						, public view_interface<move_view<Rng>> {
			using base_t = detail::ebo_box<Rng, move_view<Rng>>;
			using base_t::get;
		public:
			move_view() requires DefaultConstructible<Rng> = default;

			constexpr move_view(Rng rng)
			noexcept(std::is_nothrow_move_constructible<Rng>::value)
			: base_t{std::move(rng)} {}

			move_iterator<iterator_t<Rng>> begin() const
			requires Range<const Rng>
			{ return __stl2::make_move_iterator(__stl2::begin(get())); }
			move_sentinel<sentinel_t<Rng>> end() const
			requires Range<const Rng>
			{ return __stl2::make_move_sentinel(__stl2::end(get())); }
			move_iterator<iterator_t<Rng>> end() const
			requires Range<const Rng> && BoundedRange<const Rng>
			{ return __stl2::make_move_iterator(__stl2::end(get())); }

			auto size() const
			requires Range<const Rng> && SizedRange<const Rng>
			{ return __stl2::size(get()); }
			bool empty() const
			requires Range<const Rng> && requires(const Rng& r) { __stl2::empty(r); }
			{ return __stl2::empty(get()); }

			move_iterator<iterator_t<Rng>> begin()
			requires !Range<const Rng>
			{ return __stl2::make_move_iterator(__stl2::begin(get())); }
			move_sentinel<sentinel_t<Rng>> end()
			requires !Range<const Rng>
			{ return __stl2::make_move_sentinel(__stl2::end(get())); }
			move_iterator<iterator_t<Rng>> end()
			requires !Range<const Rng> && BoundedRange<Rng>
			{ return __stl2::make_move_iterator(__stl2::end(get())); }

			auto size()
			requires !Range<const Rng> && SizedRange<Rng>
			{ return __stl2::size(get()); }
			bool empty()
			requires !Range<const Rng>
			{ return __stl2::empty(get()); }
		};
	} // namespace ext

	template <class V>
	struct enable_view<ext::move_view<V>> : std::true_type {};

	namespace __move {
		struct fn : detail::__pipeable<fn> {
			template <InputRange Rng>
			requires std::is_lvalue_reference_v<Rng> || View<__f<Rng>>
			constexpr ext::move_view<ext::all_view<Rng>> operator()(Rng&& rng) const {
				return ext::move_view<ext::all_view<Rng>>{
					view::all(std::forward<Rng>(rng))};
			}
		};
	} // namespace __move

	namespace view {
		inline constexpr __move::fn move {};
	}
} STL2_CLOSE_NAMESPACE

#endif
