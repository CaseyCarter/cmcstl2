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
#ifndef STL2_VIEW_TAKE_EXACTLY_HPP
#define STL2_VIEW_TAKE_EXACTLY_HPP

#include <type_traits>
#include <utility>
#include <stl2/detail/ebo_box.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <View Base>
		class take_exactly_view : detail::ebo_box<Base, take_exactly_view<Base>>
		{
			using base_t = detail::ebo_box<Base, take_exactly_view<Base>>;
			using base_t::get;

			difference_type_t<iterator_t<Base>> n_;
		public:
			take_exactly_view() requires DefaultConstructible<Base>() = default;

			constexpr take_exactly_view(Base view, difference_type_t<iterator_t<Base>> n)
			noexcept(std::is_nothrow_move_constructible<Base>::value)
			: base_t{std::move(view)}, n_{n}
			{ STL2_EXPECT(n >= 0); }

			constexpr auto begin()
			noexcept(noexcept(__stl2::make_counted_iterator(__stl2::begin(std::declval<Base&>()), n_)))
			requires !Range<Base const>()
			{ return __stl2::make_counted_iterator(__stl2::begin(get()), n_); }
	#if 0
			constexpr auto data()
			noexcept(noexcept(__stl2::data(std::declval<Base&>())))
			requires
				!Range<Base const>() &&
				requires(Base& b) { __stl2::data(b); }
			{ return __stl2::data(get()); }
	#else
			template <class B = Base>
			requires
				Same<B, Base>() &&
				!Range<B const>() &&
				requires(B& b) { __stl2::data(b); }
			constexpr auto data()
			noexcept(noexcept(__stl2::data(std::declval<B&>())))
			{ return __stl2::data(get()); }
	#endif

			constexpr auto begin() const
			noexcept(noexcept(__stl2::make_counted_iterator(__stl2::begin(std::declval<Base const&>()), n_)))
			requires Range<Base const>()
			{ return __stl2::make_counted_iterator(__stl2::begin(get()), n_); }
	#if 0
			constexpr auto data() const
			noexcept(noexcept(__stl2::data(std::declval<Base const&>())))
			requires
				!Range<Base const>() &&
				requires(Base const& b) { __stl2::data(b); }
			{ return __stl2::data(get()); }
	#else
			template <class B = Base>
			requires
				Same<B, Base>() &&
				Range<B const>() &&
				requires(B const& b) { __stl2::data(b); }
			constexpr auto data() const
			noexcept(noexcept(__stl2::data(std::declval<B const&>())))
			{ return __stl2::data(get()); }
	#endif

			constexpr default_sentinel end() const noexcept { return {}; }
			constexpr difference_type_t<iterator_t<Base>> size() const noexcept { return n_; }
			constexpr bool empty() const noexcept { return n_ == 0; }
		};
	} // namespace ext

	template <class V>
	struct enable_view<ext::take_exactly_view<V>> : std::true_type {};
} STL2_CLOSE_NAMESPACE

#endif
